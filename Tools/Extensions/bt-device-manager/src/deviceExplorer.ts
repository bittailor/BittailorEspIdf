import * as vscode from 'vscode';
import * as mqtt from 'mqtt'

export class BtDeviceTreeDataProvider implements vscode.TreeDataProvider<BtDevice> {

	private _onDidChangeTreeData: vscode.EventEmitter<any> = new vscode.EventEmitter<any>();
	readonly onDidChangeTreeData: vscode.Event<any> = this._onDidChangeTreeData.event;
    
    private devices: BtDevice[];
    
	constructor() { 
        this.devices = [
            new BtDevice("One"),    
            new BtDevice("Two"),    
            new BtDevice("Three"),    
            new BtDevice("Four"),    
        ];
        

    }

	public refresh(): any {
		this.devices = [
            new BtDevice("Five"),    
            new BtDevice("Six"),    
            new BtDevice("Seven"),    
        ];
        this._onDidChangeTreeData.fire(undefined);
	}


	public getTreeItem(element: BtDevice): vscode.TreeItem {
		return {
            collapsibleState: vscode.TreeItemCollapsibleState.None,
            label: element.label,
            contextValue: "BtDevice"
        };
	}

	public getChildren(element?: BtDevice): BtDevice[] | Thenable<BtDevice[]> {
		if(element) {
            return [];
        }
        return this.devices;
	}
}

class BtDevice {
    constructor(
      public readonly label: string,
    ) {
    }

    public restart(){
        vscode.window.showInformationMessage(`restart device ${this.label}.`)
    }
}

export class BtDeviceExplorer {
	

    private ftpViewer: vscode.TreeView<BtDevice>;
    private client: mqtt.MqttClient;


	constructor(context: vscode.ExtensionContext) {
		const treeDataProvider = new BtDeviceTreeDataProvider();
	
		this.ftpViewer = vscode.window.createTreeView('btDeviceExplorer', { treeDataProvider });

        
		vscode.commands.registerCommand('btDeviceExplorer.refresh', () => treeDataProvider.refresh());
        vscode.commands.registerCommand('btDeviceExplorer.restartDevice', (device: BtDevice) => this.restartDevice(device));
		
        this.client = mqtt.connect('mqtt://piOne.local',{
            username: 'myUser',
            password: 'myPasswords'    
        })

        this.client.on('connect',  (err) => {
            console.log('on connect')
            this.client.subscribe('bittailor/home/#', (err) => {
                console.log('subscribed : ', err);
            })
        })
        
        this.client.on('message', function (topic, message) {
            console.log(message.toString())
        })
	}

    dispose() {
        this.client.end();    
	}

    private restartDevice(device: BtDevice) {
        device.restart();
    }

}
  