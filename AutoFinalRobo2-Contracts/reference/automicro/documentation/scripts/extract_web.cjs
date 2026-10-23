// Static TypeScript AST extraction only. Never imports the analyzed application.
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');
const base = path.resolve(__dirname, '..');
const root = path.resolve(base, '../../Web ESP01/WebESP01AutoMicro2025');
const ts = require(path.join(root, 'node_modules/typescript/lib/typescript.js'));
const output = path.resolve(base, '../.cache/amdoc-web-ast.json');
const hash = bytes => crypto.createHash('sha256').update(bytes).digest('hex');
const normalize = value => value.replace(/\s+/g, ' ').trim();
const requested = new Set();
for (let i=2;i<process.argv.length;i++) {
  if (process.argv[i]!=='--file' || !process.argv[i+1]) throw new Error('Usar --file src/ruta.ts (repetible)');
  requested.add(process.argv[++i].replace(/\\/g,'/'));
}
const files = [];
function walk(directory) {
  for (const entry of fs.readdirSync(directory, {withFileTypes:true}).sort((a,b)=>a.name.localeCompare(b.name))) {
    const absolute = path.join(directory, entry.name);
    const relative = path.relative(root,absolute).split(path.sep).join('/');
    if (entry.isSymbolicLink()) continue;
    if (entry.isDirectory()) { walk(absolute); continue; }
    if (!/\.(tsx?|jsx?)$/.test(entry.name)) continue;
    if (requested.size && !requested.has(relative)) continue;
    const bytes = fs.readFileSync(absolute);
    const text = bytes.toString('utf8');
    const source = ts.createSourceFile(absolute,text,ts.ScriptTarget.Latest,true,entry.name.endsWith('tsx')?ts.ScriptKind.TSX:undefined);
    const functions=[],types=[],imports=[],hooks=[];
    const line = node => source.getLineAndCharacterOfPosition(node.getStart(source)).line+1;
    const typeText = node => node ? normalize(node.getText(source)) : 'inferred-by-TypeScript';
    const params = node => node.parameters.map(p=>`${p.dotDotDotToken?'...':''}${p.name.getText(source)}${p.questionToken?'?':''}: ${typeText(p.type)}`).join(', ');
    function visit(node, scope='') {
      if (ts.isImportDeclaration(node)||ts.isExportDeclaration(node)) {
        if (node.moduleSpecifier) imports.push(node.moduleSpecifier.text);
      }
      let nextScope=scope;
      const functionLike=ts.isFunctionDeclaration(node)||ts.isMethodDeclaration(node)||ts.isArrowFunction(node)||ts.isFunctionExpression(node)||ts.isGetAccessor(node)||ts.isSetAccessor(node);
      if (functionLike) {
        let name=node.name?.getText(source);
        if (!name && ts.isVariableDeclaration(node.parent)) name=node.parent.name.getText(source);
        if (!name && ts.isPropertyAssignment(node.parent)) name=node.parent.name.getText(source);
        if (name) {
          const qualified=scope?scope+'.'+name:name;
          const calls=new Set();
          const collect = child => {
            if (child!==node && (ts.isFunctionDeclaration(child)||ts.isArrowFunction(child)||ts.isFunctionExpression(child))) return;
            if (ts.isCallExpression(child)) calls.add(normalize(child.expression.getText(source)));
            ts.forEachChild(child,collect);
          };
          if(node.body) collect(node.body);
          functions.push({name:qualified,displayName:name,signature:`${qualified}(${params(node)}): ${typeText(node.type)}`,line:line(node),endLine:source.getLineAndCharacterOfPosition(node.end).line+1,definition:!!node.body,private:!!scope,calls:[...calls].sort(),writes:[]});
          nextScope=qualified;
        }
      }
      if(ts.isCallExpression(node)) {
        const callee=normalize(node.expression.getText(source));
        if(/^use[A-Z]/.test(callee)||/^React\.use/.test(callee)) hooks.push({callee,scope,line:line(node),arguments:node.arguments.length});
      }
      if(ts.isInterfaceDeclaration(node)||ts.isTypeAliasDeclaration(node)||ts.isClassDeclaration(node)||ts.isEnumDeclaration(node)) {
        const name=node.name?.text||'anonymous';
        const members=node.members || (node.type&&ts.isTypeLiteralNode(node.type)?node.type.members:[]);
        const fields=[];
        for(const member of members) {
          if(!member.name) continue;
          const memberName=member.name.getText(source);
          const callable=ts.isMethodSignature(member)||ts.isMethodDeclaration(member)||(member.type&&ts.isFunctionTypeNode(member.type));
          const sig=(ts.isMethodSignature(member)||ts.isMethodDeclaration(member))?`${memberName}(${params(member)}): ${typeText(member.type)}`:`${memberName}${member.questionToken?'?':''}: ${typeText(member.type)}`;
          fields.push({name:memberName,declaration:sig,callback:!!callable,ownerType:name,evidence:{rootId:'web',path:relative,line:line(member)}});
        }
        types.push({name,kind:ts.SyntaxKind[node.kind],type:ts.isTypeAliasDeclaration(node)&&!ts.isTypeLiteralNode(node.type)?typeText(node.type):undefined,fields,evidence:{rootId:'web',path:relative,line:line(node)}});
        if(ts.isClassDeclaration(node)) nextScope=name;
      }
      ts.forEachChild(node,child=>visit(child,nextScope));
    }
    visit(source);
    files.push({path:relative,sha256:hash(bytes),imports:[...new Set(imports)].sort(),functions,types,hooks,analysisLevel:'typescript-ast-static'});
  }
}
walk(path.join(root,'src'));
fs.mkdirSync(path.dirname(output),{recursive:true});
const refreshed = requested.size && fs.existsSync(output)
  ? [...JSON.parse(fs.readFileSync(output,'utf8')).files.filter(f=>!requested.has(f.path)),...files].sort((a,b)=>a.path.localeCompare(b.path))
  : files;
fs.writeFileSync(output,JSON.stringify({schemaVersion:1,rootId:'web',files:refreshed},null,2)+'\n');
process.stdout.write(JSON.stringify({operation:'documentation-extraction',root:'web',files:files.length,functions:files.reduce((a,f)=>a+f.functions.length,0)})+'\n');
