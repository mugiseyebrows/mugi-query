@echo off
where pip > NUL || exit /b
where jinja2 > NUL || pip install jinja2-cli
where pbat > NUL || pip install pbat
jinja2 -D mkspec="win32-g++" template.j2 > build-mingw.pbat
jinja2 -D mkspec="win32-msvc" template.j2 > build-msvc.pbat
pbat