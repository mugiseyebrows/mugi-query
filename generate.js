
const {CppClass, cpp, qt, mName, CppSignature} = require('mugicpp')
const path = require('path')

function main() {

    let c = new CppClass('Field')
    let m = {
        name: qt.QString,
        type: qt.QString,
        size: cpp.int,
        primaryKey: cpp.bool,
        autoincrement: cpp.bool,
        index: cpp.bool,
        unique: cpp.bool,
        foreignKey: qt.QString
    }

    let d = {
        type: '"INT"',
        size: -1,
        primaryKey: false,
        autoincrement: false,
        index: false,
        unique: false,
        foreignKey: 'QString()'
    }

    let s = new CppSignature

    for(var n in m) {
        c.member(mName(n), m[n], d[n])
    }

    c.constructor_({}, mName(d))
    c.constructor_(s.signature(m,d))

    c.write(path.join(__dirname,'src'))
}

main()