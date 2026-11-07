"""AFR2-20261106-02. Offline integrity/budget check; no HTTP or hardware."""
from pathlib import Path
import json, gzip, hashlib, sys
root=Path(__file__).resolve().parents[2]
package=Path(sys.argv[1]).resolve() if len(sys.argv)>1 else root.parent/'AutoFinalRobo2-ESP/data/dist'
text=(package/'manifest.json').read_bytes()
m=json.loads(text)
assert len(text)<=12288 and 0<len(m['files'])<=24
assert m['schema']=='afr2-littlefs-manifest-1' and m['project']=='AutoFinalRobo2'
assert m['rootIndexServedPath']=='/index.html.gz'
assert len({f['route'] for f in m['files']})==len(m['files'])
for f in m['files']:
 assert f['servedPath']==f['route']+'.gz' and '..' not in f['servedPath']
 assert not f['route'].startswith(('/api/','/config/')) and f['route']!='/ws'
 compressed=(package/f['servedPath'].lstrip('/')).read_bytes()
 raw=gzip.decompress(compressed)
 assert len(compressed)==f['gzipBytes'] and len(raw)==f['rawBytes']
 assert hashlib.sha256(raw).hexdigest()==f['sha256'], f['route']
assert sum(f['gzipBytes'] for f in m['files'])==m['totalGzipBytes']
assert sum(f['rawBytes'] for f in m['files'])==m['totalRawBytes']
assert m['estimatedFilesystemBytes']<=m['filesystemBudgetBytes']
print(json.dumps({'files':len(m['files']),'manifestBytes':len(text),'gzipBytes':m['totalGzipBytes'],'estimatedFilesystemBytes':m['estimatedFilesystemBytes'],'budgetBytes':m['filesystemBudgetBytes'],'result':'pass'}))
