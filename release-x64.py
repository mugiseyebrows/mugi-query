from mugidelivery import Manager

conf = {
    'AppName' : 'mugi-query',
    'AppVersion' : '1.0.0',
    'Binaries' : [{'Source': 'D:\\dev\\mugi-query\\release\\mugi-query.exe', 'Dest': ''}],

    'BuildDir' : 'D:\\dev\\mugi-query',
    'Arch': 'win64',
    'VersionUpdater': 'QtVersionUpdater',
    'Packer': 'SimplePacker',
    
    'QtPlugins': ['qwindows','qwindowsvistastyle',"qsqlite","qsqlmysql","qsqlodbc","qsqlpsql"],
    'QtPluginsSource': 'D:\\Qt5\\Qt5.12.0\\5.12.0\\mingw73_64\\plugins'
}

# D:\Qt5\Qt5.12.0\5.12.0\mingw73_64\bin\qtenv2.bat

# cd /d/Qt5/Qt5.12.0/5.12.0/mingw73_64/plugins/sqldrivers
# ls | grep -v 'd.dll' | sed 's,\(.*\).dll,"\1",' | tr '\n' ','

m = Manager(conf)
#m.updateVersion()
#m.build()
m.pack()
#m.release()
