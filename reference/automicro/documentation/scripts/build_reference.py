"""Extrae referencias documentales; no compila, ejecuta ni valida el firmware.

Entrada editorial: documentation/overlays/f4-libraries.json.
Salida reemplazable: documentation/published/f4/ y source-map.json.
Extracción C estructural, sin preprocesador: conservar explícitos sus límites.
"""
from pathlib import Path
import argparse
import hashlib
import json
import re

BASE = Path(__file__).resolve().parents[1]
F4 = BASE.parent.parent / "FirmwareF4"
TYPE_NOTES = {}
KNOWN_FUNCTIONS = {}


def read(path):
    return path.read_text(encoding="utf-8-sig", errors="replace")


def masked(text):
    pattern = r'/\*[\s\S]*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\''
    return re.sub(pattern, lambda m: re.sub(r"[^\n]", " ", m.group()), text)


def close_brace(text, start):
    depth = 0
    for i in range(start, len(text)):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return i
    return len(text) - 1


def evidence(path, text, offset):
    return {"rootId": "f4", "path": path, "line": text.count("\n", 0, offset) + 1}


def functions(path, text):
    clean = masked(text)
    pattern = r'(?m)^[ \t]*((?:(?:[A-Za-z_]\w*|\*)[ \t\n]+)+[* \t]*)([A-Za-z_]\w*)\s*\(([^;{}]*?)\)\s*([;{])'
    result = []
    for m in re.finditer(pattern, clean):
        prefix, name, args, tail = m.groups()
        if name in {"if", "while", "switch", "for", "sizeof", "__attribute__"} or any(x in prefix.split() for x in ("return", "typedef", "define")):
            continue
        if tail == "{" and path.endswith(".h") and "inline" not in prefix:
            continue
        end = close_brace(clean, m.end() - 1) if tail == "{" else m.end()
        body = clean[m.end():end] if tail == "{" else ""
        calls = sorted(set(re.findall(r'\b([A-Za-z_]\w*)\s*\(', body)) - {"if", "for", "while", "switch", "sizeof", "return"})
        result.append({"name": name, "signature": re.sub(r'\s+', ' ', text[m.start():m.end()-1]).strip(),
                       "evidence": evidence(path, text, m.start()), "definition": tail == "{",
                       "private": "static" in prefix.split(), "calls": calls,
                       "writes": sorted(set(re.findall(r'\b\w+->([A-Za-z_]\w*)\s*(?:\+\+|--|[+*/|&^-]?=(?!=))', body))),
                       "body": body})
    return result


def declarations(path, text):
    clean = masked(text)
    result = []
    for m in re.finditer(r'\b(?:typedef\s+)?(?:struct|union)\s*([A-Za-z_]\w*)?\s*\{', clean):
        end = close_brace(clean, m.end()-1)
        suffix = re.match(r'\s*([A-Za-z_]\w*)?\s*;', clean[end+1:])
        if not suffix:
            continue
        name = suffix.group(1) or m.group(1)
        if not name:
            continue
        body = clean[m.end():end]
        fields = []
        start = 0
        depth = 0
        for i, c in enumerate(body):
            if c == "{": depth += 1
            if c == "}": depth -= 1
            if c == ";" and depth == 0:
                raw = body[start:i].strip()
                raw = re.sub(r'(?m)^\s*#.*$', '', raw).strip()
                if raw:
                    sig = re.sub(r'\s+', ' ', raw)
                    sig = re.sub(r'\{.*\}', '{ estructura anidada }', sig)
                    pointer = re.search(r'\(\*\s*(\w+)\)', sig)
                    names = pointer.group(1) if pointer else re.sub(r'\[[^\]]*\]', '', sig).split()[-1].lstrip('*')
                    fields.append({"name": names, "declaration": sig, "callback": bool(pointer), "ownerType": name,
                                   "evidence": evidence(path, text, m.end()+start)})
                start = i+1
        result.append({"name": name, "fields": fields, "evidence": evidence(path, text, m.start())})
    for m in re.finditer(r'(?m)^\s*typedef\s+([^;{}]+?\(\*\s*(\w+)\)\s*\([^;{}]*\))\s*;', clean):
        result.append({"name": m.group(2), "callbackType": re.sub(r'\s+', ' ', m.group(1)),
                       "fields": [], "evidence": evidence(path, text, m.start())})
    return result


def words(name):
    return re.sub(r'([a-z0-9])([A-Z])', r'\1 \2', name).replace('_', ' ').strip()


def describe(name, module, definition=None):
    exact = module.get("functions", {})
    if name in exact:
        return exact[name], "editorial"
    if name in KNOWN_FUNCTIONS:
        return KNOWN_FUNCTIONS[name], "editorial"
    short = name
    for prefix in module.get("prefixes", []):
        if short.startswith(prefix):
            short = short[len(prefix):]
            break
    if short in exact:
        return exact[short], "editorial"
    if definition:
        # Describe el acceso realmente extraído, sin deducir semántica del nombre.
        returned=re.findall(r'\breturn\s+(&?\w+(?:(?:->|\.)\w+)+)\s*;',definition.get('body',''))
        if returned and len(definition.get('body',''))<500 and not definition.get('writes'):
            expression=returned[-1]
            if expression.startswith('&'):
                return 'Devuelve una vista prestada del campo `'+expression[1:]+'`; sigue perteneciendo a la instancia.', 'structural'
            return 'Devuelve el campo `'+expression+'` del estado actual de la instancia.', 'structural'
    # Los adaptadores pequeños muestran la delegación que existe en su cuerpo.
    if definition and len(definition.get("body", "")) < 350:
        calls = [x for x in definition["calls"] if x not in {"memcpy", "memset", "assert_param"}]
        if len(calls) == 1 and calls[0] != name:
            return "Adapta la llamada a `" + calls[0] + "` con el contexto de esta integración.", "structural"
    for marker, action in [
        ("GetDefault", "Carga los valores predeterminados de"), ("Default", "Carga los valores predeterminados de"),
        ("Is", "Indica si se cumple la condición"), ("Has", "Indica si existe"),
        ("Get", "Consulta"), ("Set", "Configura"), ("Read", "Lee"), ("Write", "Escribe"),
        ("Request", "Solicita"), ("Report", "Publica el reporte de"),
        ("Handle", "Procesa"), ("Parse", "Decodifica"), ("Build", "Construye"),
        ("Start", "Inicia"), ("Stop", "Detiene"), ("Reset", "Restablece"),
        ("Clear", "Limpia"), ("Apply", "Aplica"), ("Send", "Envía"),
        ("Draw", "Dibuja"), ("Render", "Renderiza"), ("Update", "Actualiza"),
        ("Notify", "Notifica"), ("Flush", "Intenta despachar lo pendiente de"),
        ("On", "Atiende la notificación de"), ("Init", "Inicializa"),
        ("Bind", "Conecta los servicios de"), ("Check", "Consulta las condiciones de")]:
        if short.startswith(marker):
            subject = words(short[len(marker):]) or module["title"]
            return f"{action} {subject} dentro de {module['title']}.", "inferred-name"
    if short.endswith("_Wrapper"):
        return "Prepara el contexto de UI y selecciona la vista " + words(short[:-8]) + ".", "inferred-name"
    if short.endswith("EventManager") or short.endswith("_UserEventManager"):
        return "Despacha los eventos de usuario de " + words(short) + ".", "inferred-name"
    if definition:
        calls = definition.get("calls", [])
        if calls:
            return "Coordina " + ", ".join('`'+x+'`' for x in calls[:3]) + " para " + module["title"] + ".", "structural"
    return "Operación " + words(short) + " de " + module["title"] + "; semántica fina pendiente de ampliación.", "inferred-name"


def field_description(field, module):
    name = field["name"]
    key=field.get('ownerType','')+'.'+name
    if key in TYPE_NOTES: return TYPE_NOTES[key]
    custom = module.get("attributes", {})
    if name in custom: return custom[name]
    if field["callback"]:
        if name in CALLBACKS: return CALLBACKS[name]
        return describe(name, module)[0]
    if name in FIELDS: return FIELDS[name]
    if name.endswith("_ctx") or name in {"ctx", "callbackCtx", "send_ctx"}:
        return "Contexto opaco prestado al callback; el integrador conserva su almacenamiento."
    if "ops" in name or name in {"services", "hardware", "storage", "port", "bus", "orientation"}:
        return "Contrato de operaciones externo; conservar la tabla y su contexto durante el uso."
    if name.startswith("last_") or name.startswith("prev"):
        return "Memoria de la observación anterior usada por la máquina de estados."
    if name.endswith("_ms") or name.endswith("Ms"):
        return "Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe."
    if name.endswith("_us"):
        return "Tiempo o intervalo en microsegundos; precisión efectiva dependiente del puerto."
    if name.endswith("_deg") or name.endswith("Deg"):
        return "Ángulo en grados; respetar eje y referencia de esta librería."
    if name.endswith("_percent") or name.endswith("Percent"):
        return "Magnitud porcentual; el rango y signo se fijan en la API de esta librería."
    return "Dato de estado/configuración `"+name+"`; declaración estructural, interpretación específica pendiente de ampliar."


FIELDS = {
    "config":"Configuración copiada por la instancia; los punteros que contiene siguen siendo prestados.",
    "status":"Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento.",
    "initialized":"Marca de inicialización local; no acredita disponibilidad de dispositivos externos.",
    "flags":"Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes.",
    "trigger":"Bandera externa volátil de adquisición; no contiene una muestra ni prueba de frescura.",
    "system":"Referencia al estado compartido AutoMicro; introduce dependencia del modelo de aplicación.",
    "menuSystem":"Referencia al menú compuesto por la aplicación.",
    "wifiBridge":"Referencia al bridge concreto AutoMicro; su acceso está mediado por servicios.",
    "latest":"Último snapshot producido; incluye secuencia y datos de adquisición.",
    "sample_seq":"Secuencia de muestra; cambia al producir nuevos datos, no al volver a leerlos.",
    "on_data_ready_cb":"Aviso de muestra procesada sin argumento de contexto; lo invoca MPU9250_Process.",
    "hardware_ctx":"Recurso opaco de plataforma usado por las operaciones de hardware.",
    "hi2c":"Recurso opaco del bus; la aplicación STM32 entrega el handle HAL.",
    "htim":"Recurso opaco del PWM; la aplicación STM32 entrega el timer HAL.",
    "tx_buffer[1]":"Byte de selección de registro retenido para DMA.",
    "rx_buffer[22]":"Buffer de recepción conservado en la instancia para completar la adquisición.",
    "owner_id":"ID lógico del propietario actual del bus; NONE significa libre.",
    "last_owner_id":"Propietario previo usado en el desempate de solicitudes.",
    "priority":"Prioridad base registrada; no confundir con pending_prio ni asumir que gobierna la selección.",
    "pending_prio":"Prioridad de solicitud acumulada para el próximo arbitraje.",
    "policy":"Política de arbitraje con preferencias explícitas OLED/MPU.",
    "framebuffer":"Imagen binaria retenida durante el ensamblaje/visualización del canvas.",
    "source_node":"Identidad UNER del solicitante; no sustituir por un requestId JSON.",
    "transport_id":"Transporte de entrada/salida; forma parte de la identidad del endpoint.",
    "payload":"Vista del payload; copiar antes de permitir la reutilización del slot del parser.",
    "tx_buf":"Buffer de construcción de frames de esta instancia; no conservar una referencia tras el siguiente envío.",
    "appAuthPinDigits":"Secreto de autenticación en RAM; documentar el tipo, nunca publicar su contenido.",
}
CALLBACKS = {
    "now_ms":"Entrega reloj monotónico modular en milisegundos para deadlines y períodos.",
    "now_us":"Entrega reloj en microsegundos; el adaptador actual deriva ms × 1000.",
    "delay_ms":"Realiza una espera síncrona de inicialización; no vuelve asíncrona la configuración.",
    "ready":"Consulta si el recurso de hardware acepta una transferencia.",
    "probe":"Sondea presencia del dispositivo; el backend actual ejecuta una operación síncrona.",
    "mem_write":"Escribe un registro de forma síncrona; el llamador entrega dirección, buffer y timeout.",
    "mem_read":"Lee registros de forma síncrona al buffer provisto.",
    "mem_write_dma":"Inicia escritura DMA; conservar el buffer hasta el callback final o error.",
    "tx_dma":"Inicia transmisión DMA al dispositivo; aceptación no equivale a transferencia finalizada.",
    "rx_dma":"Inicia recepción DMA al buffer persistente del cliente.",
    "bind":"Registra identidad, dirección y callbacks del cliente en el backend I2C.",
    "request":"Solicita propiedad del bus con prioridad; la concesión llega por on_granted.",
    "release":"Libera la concesión I2C para permitir atender otros clientes.",
    "is_owner":"Consulta la propiedad del bus del cliente conectado a este puerto.",
    "policy":"Consulta la política del árbitro sin conocer su handle concreto.",
    "recover_cb":"Hook opcional para recuperar el bus tras fallo; recibe el recurso opaco de hardware.",
    "read_level":"Lee el nivel eléctrico del botón; activeLevel interpreta su polaridad.",
    "read_count":"Lee el contador bruto de 16 bits del encoder.",
    "reset_count":"Reinicia el contador físico del encoder desde su adaptador.",
    "button_level":"Lee el pulsador integrado del encoder.",
    "set_compare":"Escribe el compare del canal PWM; no aplica por sí mismo las guardas de movimiento.",
    "set_emitters":"Ordena encender/apagar un grupo IR; el adaptador resuelve los pines.",
    "view":"Expone una vista prestada del almacenamiento en una dirección/rango.",
    "unlock":"Habilita escritura en almacenamiento; puede fallar.",
    "lock":"Cierra escritura en almacenamiento tras la transacción.",
    "erase":"Borra almacenamiento de forma síncrona según el backend.",
    "program":"Programa una palabra en la dirección indicada según el backend.",
    "item_available":"Decide disponibilidad del ítem según política externa y contexto.",
    "report_screen":"Intenta publicar identidad de pantalla y origen; false conserva el reporte pendiente.",
    "report_selection":"Intenta publicar selección y cantidad; no contiene el framebuffer.",
    "on_raster":"Notifica progreso de raster del driver OLED mediante callback_ctx.",
    "on_page":"Notifica la página OLED procesada mediante callback_ctx.",
    "on_presented":"Notifica fin de presentación OLED según el driver, mediante callback_ctx.",
    "on_empty":"Notifica que la cola OLED quedó vacía mediante callback_ctx.",
    "poll_rx":"Entrega bytes del transporte al core UNER durante el polling.",
    "try_send":"Intenta aceptar un frame; BUSY exige política de reintento del llamador.",
    "forward_guard":"Autoriza o bloquea el forwarding para paquete y transporte de salida.",
    "onComplete":"Entrega el estado del teclado confirmado y el contexto del usuario.",
    "onCancel":"Informa cancelación del teclado al contexto del usuario.",
    "clearScreen":"Limpia el buffer de dibujo provisto por el integrador.",
    "setColor":"Selecciona color lógico para las primitivas de dibujo siguientes.",
    "setCursor":"Ubica el cursor de texto en coordenadas de píxel.",
    "writeChar":"Dibuja un carácter con la fuente elegida por el integrador.",
    "writeString":"Dibuja una cadena terminada en cero; no transfiere su propiedad.",
    "drawRect":"Dibuja el contorno del rectángulo en píxeles.",
    "fillRect":"Rellena el rectángulo en píxeles.",
    "drawHLine":"Dibuja un segmento horizontal en píxeles.",
    "drawVLine":"Dibuja un segmento vertical en píxeles.",
    "updateScreen":"Solicita el refresco completo al backend de dibujo.",
    "requestPageUpdate":"Solicita refresco de una página del backend de dibujo.",
}


def esc(value):
    return str(value).replace('|', '\\|').replace('\n', ' ')


def ref(e):
    return f"{e['rootId']}:{e['path']}:{e['line']}"


def run():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--module', action='append', default=[], help='ID estable o slug de ficha; omitir para generar todas')
    options=parser.parse_args()
    editorial = {"modules": []}
    for overlay in sorted((BASE/'overlays').glob('f4-*.json')):
        editorial['modules'].extend(json.loads(read(overlay))['modules'])
    TYPE_NOTES.update(json.loads(read(BASE/'overlays/field-notes.json')))
    extra_api=BASE/'overlays/api-notes.json'
    if extra_api.exists(): KNOWN_FUNCTIONS.update(json.loads(read(extra_api)))
    for m in editorial['modules']:
        for name,description in m.get('functions',{}).items():
            if '_' in name: KNOWN_FUNCTIONS[name]=description
            for prefix in m.get('prefixes',[]):
                KNOWN_FUNCTIONS[prefix+name]=description
    findings = json.loads(read(BASE/'review/findings.json'))['findings']
    selected=[m for m in editorial['modules'] if not options.module or m['id'] in options.module or m['slug'] in options.module]
    selected_paths={p for m in selected for p in m['files']}
    files = {}
    for folder in ("Core", "USB_DEVICE"):
        for path in sorted((F4/folder).rglob('*')):
            if path.suffix not in {'.h','.c'}: continue
            relative = path.relative_to(F4).as_posix()
            if options.module and relative not in selected_paths: continue
            text = read(path)
            files[relative] = {"text":text, "functions":functions(relative,text),
                               "types":declarations(relative,text),
                               "includes":re.findall(r'(?m)^\s*#\s*include\s+[<"]([^>"\n]+)',text),
                               "sha256":hashlib.sha256(path.read_bytes()).hexdigest()}
    out = BASE/'published/f4/libraries'
    out.mkdir(parents=True,exist_ok=True)
    index = []
    for module in selected:
        owned = module['files']
        by_name = {}
        internals = []
        types = []
        for path in owned:
            item=files[path]
            types.extend(item['types'])
            for fn in item['functions']:
                if fn['private'] and path.endswith('.c'):
                    internals.append(fn)
                else:
                    old=by_name.get(fn['name'])
                    if old is None or (old['definition'] and path.endswith('.h')):
                        by_name[fn['name']]=fn.copy()
        internal_by_name={}
        for fn in internals:
            key=(fn['evidence']['path'],fn['name'])
            if key not in internal_by_name or fn['definition']:
                internal_by_name[key]=fn
        internals=list(internal_by_name.values())
        definitions = {f['name']:f for path in owned for f in files[path]['functions'] if f['definition']}
        api=[]
        for name,f in sorted(by_name.items()):
            d=definitions.get(name)
            description,origin=describe(name,module,d)
            f={k:v for k,v in f.items() if k!='body'}
            f.update(description=description,descriptionOrigin=origin,
                     implementation=d['evidence'] if d else None,
                     calls=d['calls'] if d else [],writes=d['writes'] if d else [],
                     linkage='extern-no-header' if f['evidence']['path'].endswith('.c') else 'header')
            api.append(f)
        for fn in internals:
            fn['description'],fn['descriptionOrigin']=describe(fn['name'],module,fn)
            fn.pop('body',None)
        for typ in types:
            for field in typ['fields']: field['description']=field_description(field,module)
        model={"schemaVersion":1,"format":"amdoc-1","id":module['id'],"slug":module['slug'],
               "title":module['title'],"target":"f4","kind":module['kind'],
               "status":"source-documented","verification":"not-run-user-instruction",
               "snapshotDate":"2026-10-20","responsibility":module['responsibility'],
               "scope":module['scope'],"state":module['state'],"lifecycle":module['lifecycle'],
               "interactions":module['interactions'],"limitations":module['limitations'],
               "modularity":module['modularity'],
               "files":[{"path":p,"rootId":"f4","sha256":files[p]['sha256'],"includes":files[p]['includes']} for p in owned],
               "findings":[f['id'] for f in findings if module['id'] in f['modules']],
               "types":types,"functions":api,"internalFunctions":sorted(internals,key=lambda x:(x['evidence']['path'],x['evidence']['line']))}
        (out/(module['slug']+'.json')).write_text(json.dumps(model,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
        lines=[f"# {module['title']}","",f"ID: `{module['id']}` · Capa: **{module['kind']}** · Fotografía: 2026-10-20.","",
               "Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.","",
               "## Responsabilidad y alcance","",module['responsibility'],"",module['scope'],"",
               '<a id="modularity"></a>', "## Modularidad observada","",module['modularity'],"",
               '<a id="state"></a>', "## Handle, propiedad y exposición de atributos","",module['state'],"",
               '<a id="lifecycle"></a>', "## Ciclo de vida y ejecución","",*['- '+x for x in module['lifecycle']],"",
               "## Trabajo en tándem","",*['- '+x for x in module['interactions']],"",
               "## Precauciones y límites","",*['- '+x for x in module['limitations']],"",
               "## Hallazgos vinculados","", *['- ['+f['id']+': '+f['title']+'](../../../review/FINDINGS.md#'+f['id'].lower()+')' for f in findings if module['id'] in f['modules']], "",
               "## Archivos y dependencias de compilación","","| Archivo | Includes directos | SHA-256 |","| --- | --- | --- |"]
        for f in model['files']:
            lines.append(f"| `{f['path']}` | {esc(', '.join('`'+x+'`' for x in f['includes']))} | `{f['sha256']}` |")
        lines += ["","## Tipos, atributos y punteros a función","",
                  "Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.",""]
        seen_types=set()
        for typ in types:
            key=(typ['name'],typ['evidence']['path'],typ['evidence']['line'])
            if key in seen_types: continue
            seen_types.add(key)
            lines += ['### `'+typ['name']+'`', '', '`'+ref(typ['evidence'])+'`', '']
            if 'callbackType' in typ:
                lines += ['Firma: `'+typ['callbackType']+'`.',
                          CALLBACKS.get(typ['name'], 'Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.'),'']
            if typ['fields']:
                lines += ['| Atributo / firma | Función y precaución |','| --- | --- |']
                lines += ['| `'+esc(f['declaration'])+'` | '+esc(f['description'])+' |' for f in typ['fields']]
                lines += ['']
        lines += ['## Funciones públicas y símbolos externos','',
                  'Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.','',
                  '| Función y firma | Descripción breve | Evidencia / descripción |','| --- | --- | --- |']
        for f in api:
            desc=f['description']
            if not f['implementation']: desc+=' Implementación no localizada en los archivos de esta ficha.'
            lines.append('| <a id="fn-'+f['name'].lower()+'"></a>`'+esc(f['signature'])+'` | '+esc(desc)+' | `'+ref(f['evidence'])+'`; '+f['descriptionOrigin']+' |')
        if internals:
            lines += ['','## Funciones internas','', '| Función | Descripción breve | Evidencia |','| --- | --- | --- |']
            for f in model['internalFunctions']:
                lines.append('| <a id="fn-'+f['name'].lower()+'"></a>`'+esc(f['signature'])+'` | '+esc(f['description'])+' | `'+ref(f['evidence'])+'`; '+f['descriptionOrigin']+' |')
        (out/(module['slug']+'.md')).write_text('\n'.join(lines)+'\n',encoding='utf-8')
        index.append({"id":module['id'],"slug":module['slug'],"title":module['title'],"kind":module['kind'],
                      "json":"libraries/"+module['slug']+'.json',"markdown":"libraries/"+module['slug']+'.md',
                      "functions":len(api),"internalFunctions":len(internals),"files":owned})
    source_map=[]
    owners={p:[] for p in files}
    for m in index:
        for p in m['files']: owners[p].append(m['id'])
    for p,f in files.items():
        source_map.append({"rootId":"f4","path":p,"sha256":f['sha256'],"modules":owners[p],
                           "coverage":"structural" if owners[p] else "support-inventory",
                           "reason":None if owners[p] else "Soporte de tipos, recursos o plataforma; ver inventario, sin ficha narrativa individual."})
    if options.module and (BASE/'published/f4/index.json').exists():
        previous=json.loads(read(BASE/'published/f4/index.json'))
        changed_ids={m['id'] for m in index}
        index=[m for m in previous if m['id'] not in changed_ids]+index
        index.sort(key=lambda m:m['id'])
        existing_map=json.loads(read(BASE/'published/f4/source-map.json'))
        refreshed={row['path']:row for row in source_map}
        for row in existing_map:
            if row['path'] in refreshed:
                refreshed[row['path']]['modules']=row['modules']
        source_map=[refreshed.pop(row['path'],row) for row in existing_map]+list(refreshed.values())
    (BASE/'published/f4/index.json').write_text(json.dumps(index,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    if source_map is not None:
        (BASE/'published/f4/source-map.json').write_text(json.dumps(source_map,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    lines=['# Bibliotecas y composición de FirmwareF4','','Fotografía de trabajo: 2026-10-20. Revisión documental por lectura, sin ejecución de firmware.','',
           'Ver [arquitectura y límites](architecture.md), [metodología](../../METHOD.md) y [contratos entre repositorios](../shared/wire-contracts.md).','',
           '| Librería / unidad | Capa | Funciones externas | Internas |','| --- | --- | --- |']
    lines += [f"| [{m['title']}](libraries/{m['slug']}.md) | {m['kind']} | {m['functions']} | {m['internalFunctions']} |" for m in index]
    lines += ['','Los tipos auxiliares compartidos y el código de placa figuran en `source-map.json`. Los encabezados de puertos se incluyen en sus fichas consumidoras o en la ficha de adaptadores. Vendor HAL/CMSIS, middleware ST, bootloader separado, binarios y firmware F103 quedan fuera del manual de librerías propias.','']
    (BASE/'published/f4/README.md').write_text('\n'.join(lines),encoding='utf-8')
    from render_findings import render
    render()
    print(json.dumps({"operation":"documentation-generation","modules":len(index),"files":len(files),
                      "externalFunctions":sum(x['functions'] for x in index),"internalFunctions":sum(x['internalFunctions'] for x in index)},ensure_ascii=False))


if __name__=='__main__':
    run()
