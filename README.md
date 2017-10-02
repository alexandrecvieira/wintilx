### Wintilx - Window Title Plugin for LXPanel
### Wintilx é um plugin que mostra o título da janela ativa no LXPanel

Este plugin é baseado no código produzido por [Matej Kollar](https://github.com/xkollar) em https://github.com/xkollar/lxpanel-window-title

A linguagem é C.

O plugin será instalado em: <b>/usr/lib/x86_64-linux-gnu/lxpanel/plugins</b>

Tutorial referência: http://wiki.lxde.org/en/How_to_write_plugins_for_LXPanel

##### Dependências:
	Para Ubuntu e Debian Stretch:
	sudo apt-get install lxpanel-dev libglib2.0-dev libgtk2.0-dev libfm-dev

	Para Debian Jessie:
	sudo apt-get install libglib2.0-dev libgtk2.0-dev libfm-dev
	
#### Para instalar pelo source:
##### Importante: O path: /usr/lib/x86_64-linux-gnu/lxpanel/plugins é o local de instalação dos plugins do LXPanel
##### Usei o Eclipse Neon(4.6) para implementar o projeto
	git clone https://github.com/acamargovieira/wintilx.git
	cd wintilx
	./configure --prefix=/usr --libdir=/usr/lib/x86_64-linux-gnu/lxpanel/plugins
	make
	sudo make install
	
#### Para adicionar ao LXPanel:
	
![wintilx-add](https://user-images.githubusercontent.com/20074560/30779181-9473e258-a0bf-11e7-9d68-2b42aad76270.png)

##### Então você terá o título da janela ativa aparecendo no LXPanel:
	
![wintilx](https://user-images.githubusercontent.com/20074560/31030072-66c0fc28-a52a-11e7-8394-cd397e242325.gif)

##### Testado no LXPanel versões 0.7.2 | 0.8.2 | 0.9.3 
