"""Lector/escritor de s-expressions para las fuentes generadas de esta PCB."""
import json,re
class Quoted(str):pass
def parse(text):
    tokens=re.findall(r'"(?:\\.|[^"\\])*"|[^\s()]+|[()]',text)
    stack=[];root=None
    for tok in tokens:
        if tok=='(':
            n=[]
            if stack:stack[-1].append(n)
            else:root=n
            stack.append(n)
        elif tok==')':stack.pop()
        else:stack[-1].append(Quoted(json.loads(tok)) if tok.startswith('"') else tok)
    return root
def atom(v):
    if isinstance(v,Quoted):return json.dumps(v,ensure_ascii=False)
    return str(v)
def dump(node,level=0):
    if not isinstance(node,list):return atom(node)
    if not any(isinstance(v,list) for v in node):return '('+' '.join(atom(v) for v in node)+')'
    prefix=[];children=[]
    for v in node:
        if isinstance(v,list):children.append(dump(v,level+1))
        else:prefix.append(atom(v))
    return '('+' '.join(prefix)+'\n'+'\n'.join('  '*(level+1)+v for v in children)+'\n'+'  '*level+')'
def children(n,key):return [v for v in n if isinstance(v,list) and v and v[0]==key]
def child(n,key):return next((v for v in children(n,key)),None)
