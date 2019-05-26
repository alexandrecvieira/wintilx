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
	git clone https://github.com/alexandrecvieira/wintilx.git
	cd wintilx
	autoreconf -f
	./configure --prefix=/usr --libdir=/usr/lib/x86_64-linux-gnu/lxpanel/plugins
	make
	sudo make install
	
#### Para adicionar ao LXPanel:
	
![wintilx-add](http://alexandrecvieira.droppages.com/images/wintilx/wintilx-add-to-panel.png)

##### Então você terá o título da janela ativa aparecendo no LXPanel:
	
![wintilx](http://alexandrecvieira.droppages.com/images/wintilx/wintilx.gif)

##### Testado no LXPanel versões 0.7.2 | 0.8.2 | 0.9.3 

## CHANGELOG

### [Unreleased]
#### Fixed
- warning: assignment makes pointer from integer without a cast [-Wint-conversion]

### [1.0.1] - 2017-10-02
#### Fixed
- wintilx.c (lxps_event_filter): Added: Active window detection
- Nome que aparece na janela Propriedades do Painel mudado para Window Title

#### Added
- About info

### [1.0.0] - 2017-10-02
#### Removed
- Mostra versão junto do nome ao pressionar botão direito do mouse

### [0.0.4] - 2017-10-01 [YANKED]

### [0.0.3] - 2017-09-25 [YANKED]

### [0.0.2] - 2017-09-24 [YANKED]
