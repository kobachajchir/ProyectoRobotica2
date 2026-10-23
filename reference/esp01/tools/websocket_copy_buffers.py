"""Keep HTTP/WebSocket TCP buffers valid with ESPAsyncTCP on ESP8266."""
from pathlib import Path

Import("env")


def patch_tcp_buffers(build_env, node):
    # PlatformIO passes a VariantDir build node; resolve its actual source.
    source = Path(node.srcnode().get_abspath())
    original_content = source.read_bytes()
    content = original_content
    if source.name == "AsyncWebSocket.cpp":
        replacements = (
            (b'client->add((const char *)buf, headLen)',
             b'client->add((const char *)buf, headLen, ASYNC_WRITE_FLAG_COPY)'),
            (b'client->add((const char *)data, len)',
             b'client->add((const char *)data, len, ASYNC_WRITE_FLAG_COPY)'),
            (b'request->client()->write(out.c_str(), _headLength)',
             b'request->client()->write(out.c_str(), _headLength, ASYNC_WRITE_FLAG_COPY)'),
        )
    else:
        replacements = (
            (b'request->client()->add(_assembled_headers.c_str() + _writtenHeadersLength, _assembled_headers.length() - _writtenHeadersLength)',
             b'request->client()->add(_assembled_headers.c_str() + _writtenHeadersLength, _assembled_headers.length() - _writtenHeadersLength, ASYNC_WRITE_FLAG_COPY)'),
            (b'request->client()->write(_content.c_str() + _sentLength, _content.length() - _sentLength)',
             b'request->client()->write(_content.c_str() + _sentLength, _content.length() - _sentLength, ASYNC_WRITE_FLAG_COPY)'),
            (b'request->client()->add(reinterpret_cast<char *>(_send_buffer->data() + _send_buffer_offset), _send_buffer_len - _send_buffer_offset)',
             b'request->client()->add(reinterpret_cast<char *>(_send_buffer->data() + _send_buffer_offset), _send_buffer_len - _send_buffer_offset, ASYNC_WRITE_FLAG_COPY)'),
        )
    for original, patched in replacements:
        if original in content:
            content = content.replace(original, patched)
        elif patched not in content:
            raise RuntimeError("TCP-copy patch needs updating for " + source.name)

    if source.name == "WebResponses.cpp":
        # Count every accepted byte, even a partial write. A final file block
        # must stay in RESPONSE_CONTENT until all pending bytes enter TCP.
        # Preserve original line endings so dependency anchor matching is stable.
        newline = b"\r\n" if b"\r\n" in content else b"\n"
        normalized = content.replace(b"\r\n", b"\n")
        original = b"""        if (added_len != _send_buffer_len - _send_buffer_offset) {
          // we were not able to add entire buffer's content to tcp buffs, leave it for later
          // (this should not happen normally unless connection's TCP window suddenly changed from remote or mem pressure)
          _send_buffer_offset += added_len;
          break;
        } else {
          _send_buffer_len = _send_buffer_offset = 0;  // consider buffer empty
        }
        payloadlen += added_len;"""
        patched = b"""        payloadlen += added_len;  // include partial TCP writes
        if (added_len != _send_buffer_len - _send_buffer_offset) {
          _send_buffer_offset += added_len;
          // The final read may have set RESPONSE_END, but bytes are pending.
          _state = RESPONSE_CONTENT;
          break;
        } else {
          _send_buffer_len = _send_buffer_offset = 0;  // consider buffer empty
        }"""
        if original in normalized:
            normalized = normalized.replace(original, patched)
        elif patched not in normalized:
            raise RuntimeError("HTTP partial-write patch needs updating for this dependency")
        content = normalized.replace(b"\n", newline)
    if content != original_content:
        source.write_bytes(content)
    return node


# Dependencies exist when PlatformIO constructs their compilation nodes.
env.AddBuildMiddleware(patch_tcp_buffers, "*AsyncWebSocket.cpp")
env.AddBuildMiddleware(patch_tcp_buffers, "*WebResponses.cpp")
