from mugidelivery import Manager
import os

plugins = [p for p in [
    'C:\\Qt5\\5.11.1\\mingw53_32\\plugins',
    'D:\\qt\\Qt5.11.3\\5.11.3\\mingw53_32\\plugins'
] if os.path.exists(p)]

conf = {
    'AppName' : 'mugi-query',
    'AppVersion' : '1.0.1',
    'Binaries' : [{'Source': 'D:\\dev\\mugi-query\\release\\mugi-query.exe', 'Dest': ''}],

    'BuildDir' : 'D:\\dev\\mugi-query',
     'BuildActions': [{
        'cmds': [
            ['qmake'],
            ['mingw32-make.exe', 'clean'],
            ['mingw32-make.exe', 'release', '-j4']
        ],
        'cwd': 'D:\\dev\\mugi-query'
    }],
    
    'Arch': 'win32',
    'VersionUpdater': 'QtVersionUpdater',
    'Builder': 'QtBuilder',
    'Packer': 'SimplePacker',
    'VersionHeader': 'D:\\dev\\mugi-query\\src\\version.h',
    
    'Data': [],
    
    'QtPlugins': ['qwindows','qwindowsvistastyle',"qsqlite","qsqlmysql","qsqlodbc","qsqlpsql"],
    'QtPluginsSource': plugins[0]
}

#  ls 'C:\qt5\5.11.1\mingw53_32\plugins\sqldrivers' | sed 's,.dll,,' | grep -v 'd$' | sed 's,\(.*\),"\1",' | tr '\n' ','

if os.path.exists('D:\\Qt\\Qt5.11.3\\5.11.3\\mingw53_32\\bin'):
    os.environ['PATH'] = 'D:\\Qt\\Qt5.11.3\\5.11.3\\mingw53_32\\bin;D:\\Qt\\Qt5.11.3\\Tools\\mingw530_32\\bin;C:\\qwt6\\lib;C:\\lib-x86;C:\\windows\\system32'

m = Manager(conf)
m.updateVersion()
m.build()
m.pack()
#m.release()
