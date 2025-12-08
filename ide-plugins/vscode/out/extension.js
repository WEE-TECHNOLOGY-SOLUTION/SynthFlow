"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.activate = activate;
exports.deactivate = deactivate;
const path = require("path");
const vscode_1 = require("vscode");
const node_1 = require("vscode-languageclient/node");
let client;
function activate(context) {
    // Get server path from config or default to relative path from workspace root
    const config = vscode_1.workspace.getConfiguration('synthflow');
    let serverPath = config.get('lsp.serverPath');
    if (!serverPath) {
        // Fallback: assume we are running in the workspace and lsp is at root
        serverPath = path.join(context.extensionPath, '..', '..', 'synthflow-lsp.exe');
    }
    else if (!path.isAbsolute(serverPath)) {
        // Resolve relative to extension path or workspace? 
        // Better to resolve relative to workspace folder if available, or extension folder
        // For this setup, assuming relative to workspace root if opened there, 
        // but for extension distribution, it should be bundled.
        // Let's resolve relative to extension path for now if it looks relative.
        serverPath = path.resolve(context.extensionPath, serverPath);
    }
    console.log(`SynthFlow LSP Server Path: ${serverPath}`);
    // If the extension is launched in debug mode then the debug server options are used
    // Otherwise the run options are used
    const serverOptions = {
        run: { command: serverPath, transport: node_1.TransportKind.stdio },
        debug: { command: serverPath, transport: node_1.TransportKind.stdio }
    };
    // Options to control the language client
    const clientOptions = {
        // Register the server for plain text documents
        documentSelector: [{ scheme: 'file', language: 'synthflow' }],
        synchronize: {
            // Notify the server about file changes to '.clientrc files contained in the workspace
            fileEvents: vscode_1.workspace.createFileSystemWatcher('**/.clientrc')
        }
    };
    // Create the language client and start the client.
    client = new node_1.LanguageClient('synthflowLanguageServer', 'SynthFlow Language Server', serverOptions, clientOptions);
    // Start the client. This will also launch the server
    client.start();
}
function deactivate() {
    if (!client) {
        return undefined;
    }
    return client.stop();
}
//# sourceMappingURL=extension.js.map