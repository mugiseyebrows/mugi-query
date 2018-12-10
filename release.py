from mugidelivery import Manager

conf = {
    'AppName' : 'mugi-query',
    'AppVersion' : '1.0.0',
    'Binaries' : [{'Source': 'D:\\dev\\mugi-query\\release\\mugi-query.exe', 'Dest': ''}],

    'BuildDir' : 'D:\\dev\\mugi-query',
    
    'Arch': 'win32',
    'VersionUpdater': 'QtVersionUpdater',
    'Packer': 'SimplePacker',
    'Packer': 'SimplePacker',
    
    'Data': [],
    
    'QtPlugins': ['qwindows','qwindowsvistastyle',"qsqlite","qsqlmysql","qsqlodbc","qsqlpsql"],
    'QtPluginsSource': 'C:\\Qt5\\5.11.1\\mingw53_32\\plugins'
}

#  ls 'C:\qt5\5.11.1\mingw53_32\plugins\sqldrivers' | sed 's,.dll,,' | grep -v 'd$' | sed 's,\(.*\),"\1",' | tr '\n' ','

m = Manager(conf)
m.updateVersion()
#m.build()
m.pack()
#m.release()
