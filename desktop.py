
import os
import shutil

name = 'mugi-query'

desktop = os.path.expanduser('~/.local/share/applications/%s.desktop' % name)
sizes = ['128x128','16x16','256x256','32x32','48x48','512x512']

icons = os.path.expanduser('~/.local/share/icons/hicolor')
for size in sizes:
    src = os.path.join('./icons',size,'%s.png' % name)
    dst = os.path.join(icons,size,'apps','%s.png' % name)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copy(src,dst)

# /usr/bin/bash -c "LD_LIBRARY_PATH=/usr/local/lib:/usr/local/qwt-6.3.0-dev/lib /usr/local/bin/mugi-query"

icon = dst = os.path.join(icons,'256x256','apps','%s.png' % name)
executable = '/usr/bin/bash -c "LD_LIBRARY_PATH=/usr/local/lib:/usr/local/qwt-6.3.0-dev/lib /usr/local/bin/mugi-query"'
categories = ['Utility']
with open(desktop,'w') as f:
    f.write('[Desktop Entry]\nName=%s\nExec=%s\nIcon=%s\nType=Application\nCategories=%s' % (name,executable,icon,';'.join(categories) + ';'))