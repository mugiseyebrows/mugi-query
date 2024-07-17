#include "tokens.h"
#include "tokens.h"
#include "tokens.h"

#include <QSet>
#include <QSqlRecord>
#include <QSqlField>
#include <QDebug>
#include "drivernames.h"
#include <algorithm>

QStringList tableFields(QSqlDatabase db, const QString& table) {
    QSqlRecord record = db.record(table);
    int n = record.count();
    QStringList res;
    for (int i=0;i<n;i++) {
        QString field = record.fieldName(i);
        res << QString(field).toLower();
    }
    return res;
}

Tokens::Tokens()
{

}

#ifdef Q_OS_WIN

#if defined (Q_OS_WIN32)
#include <QtCore/qt_windows.h>
#endif
#if defined(Q_CC_BOR)
// workaround for Borland to make sure that SQLBIGINT is defined
#  define _MSC_VER 900
#endif
#include <sql.h>
#if defined(Q_CC_BOR)
#  undef _MSC_VER
#endif
#include <sqlext.h>

QString odbcType(const QSqlField& field, QString& error) {
// https://docs.microsoft.com/ru-ru/sql/odbc/reference/appendixes/sql-data-types?view=sql-server-2017

    QMap<int,QString> m;
    m[SQL_DECIMAL] = "DECIMAL";
    m[SQL_NUMERIC] = "NUMERIC";
    m[SQL_REAL] = "REAL";
    m[SQL_FLOAT] = "FLOAT";
    m[SQL_DOUBLE] = "DOUBLE";
    m[SQL_SMALLINT] = "SMALLINT";
    m[SQL_INTEGER] = "INTEGER";
    m[SQL_BIT] = "BIT";
    m[SQL_TINYINT] = "TINYINT";
    m[SQL_BIGINT] = "BIGINT";
    m[SQL_BINARY] = "BINARY";
    m[SQL_VARBINARY] = "VARBINARY";
    m[SQL_LONGVARBINARY] = "LONG VARBINARY";
    m[SQL_DATE] = "DATE";
    m[SQL_TYPE_DATE] = "DATE";
    m[SQL_TIME] = "TIME";
    m[SQL_TYPE_TIME] = "TIME";
    m[SQL_TIMESTAMP] = "TIMESTAMP";
    m[SQL_TYPE_TIMESTAMP] = "TIMESTAMP";
    m[SQL_WCHAR] = "WCHAR";
    m[SQL_WVARCHAR] = "VARWCHAR";
    m[SQL_WLONGVARCHAR] = "LONGWVARCHAR";
    m[SQL_CHAR] = "CHAR";
    m[SQL_VARCHAR] = "VARCHAR";
    m[SQL_GUID] = "GUID";
    m[SQL_LONGVARCHAR] = "LONG VARCHAR";

    QSet<int> hasLength;
    hasLength << SQL_CHAR << SQL_VARCHAR << SQL_WCHAR << SQL_WVARCHAR << SQL_BINARY << SQL_VARBINARY;

    int typeId = field.typeID();
    if (!m.contains(typeId)) {
        error = QString("unexpected field.typeID() %1").arg(typeId);
        return QString();
    }
    QString type = m[typeId];
    if (hasLength.contains(typeId)) {
        type = QString("%1(%2)").arg(type).arg(field.length());
    }
    return type;
}

#else

QString odbcType(const QSqlField& , QString& ) {
    return QString();
}

#endif

Tokens::Tokens(QSqlDatabase db)
{
    QStringList tableNames = db.tables();
    foreach(const QString& tableName, tableNames) {
        Table table;
        table.table = tableName.toLower();
        table.fields = tableFields(db, tableName);
        mTables.append(table);
    }

#if 0
    if (!tableNames.isEmpty()) {
        QString table = tableNames[rand() % tableNames.size()];
        QSqlRecord r = db.record(table);
        qDebug() << table;
        for(int i=0;i<r.count();i++) {
            QString error;
            QSqlField field = r.field(i);
            qDebug() << field.name() << odbcType(field,error);
            if (!error.isEmpty()) {
                qDebug() << error;
            }
        }
    }
#endif

    mDriverName = db.driverName();
}

QStringList Tokens::functions() const
{
    QStringList res;

    if (mDriverName == DRIVER_MYSQL) {

        /*
           https://dev.mysql.com/doc/refman/8.0/en/func-op-summary-ref.html
           var fns = Array.from(new Set([...document.querySelectorAll('a.link code.literal')].map( e => e.innerText ).filter(e => e.match(/[(]/) ).map( e => e.match(/^([^(]+)/)[1].toLowerCase() )))
           document.body.innerText = fns.map( (e,i) => ` << "${e}"` + ((i % 5) == 4 ? "\n" : "") ).join('')
        */

        res  << "abs" << "acos" << "adddate" << "addtime" << "aes_decrypt"
             << "aes_encrypt" << "any_value" << "ascii" << "asin" << "asymmetric_decrypt"
             << "asymmetric_derive" << "asymmetric_encrypt" << "asymmetric_sign" << "asymmetric_verify" << "atan"
             << "atan2" << "avg" << "benchmark" << "bin" << "bin_to_uuid"
             << "bit_and" << "bit_count" << "bit_length" << "bit_or" << "bit_xor"
             << "can_access_column" << "can_access_database" << "can_access_table" << "can_access_view" << "cast"
             << "ceil" << "ceiling" << "char" << "char_length" << "character_length"
             << "charset" << "coalesce" << "coercibility" << "collation" << "compress"
             << "concat" << "concat_ws" << "connection_id" << "conv" << "convert"
             << "convert_tz" << "cos" << "cot" << "count" << "crc32"
             << "create_asymmetric_priv_key" << "create_asymmetric_pub_key" << "create_dh_parameters" << "create_digest" << "cume_dist"
             << "curdate" << "current_date" << "current_role" << "current_time" << "current_timestamp"
             << "current_user" << "curtime" << "database" << "date" << "date_add"
             << "date_format" << "date_sub" << "datediff" << "day" << "dayname"
             << "dayofmonth" << "dayofweek" << "dayofyear" << "decode" << "default"
             << "degrees" << "dense_rank" << "des_decrypt" << "des_encrypt" << "elt"
             << "encode" << "encrypt" << "exp" << "export_set" << "extract"
             << "extractvalue" << "field" << "find_in_set" << "first_value" << "floor"
             << "format" << "found_rows" << "from_base64" << "from_days" << "from_unixtime"
             << "geomcollection" << "geometrycollection" << "get_dd_column_privileges" << "get_dd_create_options" << "get_dd_index_sub_part_length"
             << "get_format" << "get_lock" << "greatest" << "group_concat" << "grouping"
             << "gtid_subset" << "gtid_subtract" << "hex" << "hour" << "icu_version"
             << "if" << "ifnull" << "in" << "inet_aton" << "inet_ntoa"
             << "inet6_aton" << "inet6_ntoa" << "insert" << "instr" << "internal_auto_increment"
             << "internal_avg_row_length" << "internal_check_time" << "internal_checksum" << "internal_data_free" << "internal_data_length"
             << "internal_dd_char_length" << "internal_get_comment_or_error" << "internal_get_view_warning_or_error" << "internal_index_column_cardinality" << "internal_index_length"
             << "internal_keys_disabled" << "internal_max_data_length" << "internal_table_rows" << "internal_update_time" << "interval"
             << "is_free_lock" << "is_ipv4" << "is_ipv4_compat" << "is_ipv4_mapped" << "is_ipv6"
             << "is_used_lock" << "is_uuid" << "isnull" << "json_array" << "json_array_append"
             << "json_array_insert" << "json_arrayagg" << "json_contains" << "json_contains_path" << "json_depth"
             << "json_extract" << "json_insert" << "json_keys" << "json_length" << "json_merge"
             << "json_merge_patch" << "json_merge_preserve" << "json_object" << "json_objectagg" << "json_pretty"
             << "json_quote" << "json_remove" << "json_replace" << "json_search" << "json_set"
             << "json_storage_free" << "json_storage_size" << "json_table" << "json_type" << "json_unquote"
             << "json_valid" << "lag" << "last_insert_id" << "last_value" << "lcase"
             << "lead" << "least" << "left" << "length" << "linestring"
             << "ln" << "load_file" << "localtime" << "localtimestamp" << "locate"
             << "log" << "log10" << "log2" << "lower" << "lpad"
             << "ltrim" << "make_set" << "makedate" << "maketime" << "master_pos_wait"
             << "max" << "mbrcontains" << "mbrcoveredby" << "mbrcovers" << "mbrdisjoint"
             << "mbrequals" << "mbrintersects" << "mbroverlaps" << "mbrtouches" << "mbrwithin"
             << "md5" << "microsecond" << "mid" << "min" << "minute"
             << "mod" << "month" << "monthname" << "multilinestring" << "multipoint"
             << "multipolygon" << "name_const" << "not in" << "now" << "nth_value"
             << "ntile" << "nullif" << "oct" << "octet_length" << "ord"
             << "password" << "percent_rank" << "period_add" << "period_diff" << "pi"
             << "point" << "polygon" << "position" << "pow" << "power"
             << "quarter" << "quote" << "radians" << "rand" << "random_bytes"
             << "rank" << "regexp_instr" << "regexp_like" << "regexp_replace" << "regexp_substr"
             << "release_all_locks" << "release_lock" << "repeat" << "replace" << "reverse"
             << "right" << "roles_graphml" << "round" << "row_count" << "row_number"
             << "rpad" << "rtrim" << "schema" << "sec_to_time" << "second"
             << "session_user" << "sha1" << "sha" << "sha2" << "sign"
             << "sin" << "sleep" << "soundex" << "space" << "sqrt"
             << "st_area" << "st_asbinary" << "st_aswkb" << "st_asgeojson" << "st_astext"
             << "st_aswkt" << "st_buffer" << "st_buffer_strategy" << "st_centroid" << "st_contains"
             << "st_convexhull" << "st_crosses" << "st_difference" << "st_dimension" << "st_disjoint"
             << "st_distance" << "st_distance_sphere" << "st_endpoint" << "st_envelope" << "st_equals"
             << "st_exteriorring" << "st_geohash" << "st_geomcollfromtext" << "st_geometrycollectionfromtext" << "st_geomcollfromtxt"
             << "st_geomcollfromwkb" << "st_geometrycollectionfromwkb" << "st_geometryn" << "st_geometrytype" << "st_geomfromgeojson"
             << "st_geomfromtext" << "st_geometryfromtext" << "st_geomfromwkb" << "st_geometryfromwkb" << "st_interiorringn"
             << "st_intersection" << "st_intersects" << "st_isclosed" << "st_isempty" << "st_issimple"
             << "st_isvalid" << "st_latfromgeohash" << "st_latitude" << "st_length" << "st_linefromtext"
             << "st_linestringfromtext" << "st_linefromwkb" << "st_linestringfromwkb" << "st_longfromgeohash" << "st_longitude"
             << "st_makeenvelope" << "st_mlinefromtext" << "st_multilinestringfromtext" << "st_mlinefromwkb" << "st_multilinestringfromwkb"
             << "st_mpointfromtext" << "st_multipointfromtext" << "st_mpointfromwkb" << "st_multipointfromwkb" << "st_mpolyfromtext"
             << "st_multipolygonfromtext" << "st_mpolyfromwkb" << "st_multipolygonfromwkb" << "st_numgeometries" << "st_numinteriorring"
             << "st_numinteriorrings" << "st_numpoints" << "st_overlaps" << "st_pointfromgeohash" << "st_pointfromtext"
             << "st_pointfromwkb" << "st_pointn" << "st_polyfromtext" << "st_polygonfromtext" << "st_polyfromwkb"
             << "st_polygonfromwkb" << "st_simplify" << "st_srid" << "st_startpoint" << "st_swapxy"
             << "st_symdifference" << "st_touches" << "st_transform" << "st_union" << "st_validate"
             << "st_within" << "st_x" << "st_y" << "statement_digest" << "statement_digest_text"
             << "std" << "stddev" << "stddev_pop" << "stddev_samp" << "str_to_date"
             << "strcmp" << "subdate" << "substr" << "substring" << "substring_index"
             << "subtime" << "sum" << "sysdate" << "system_user" << "tan"
             << "time" << "time_format" << "time_to_sec" << "timediff" << "timestamp"
             << "timestampadd" << "timestampdiff" << "to_base64" << "to_days" << "to_seconds"
             << "trim" << "truncate" << "ucase" << "uncompress" << "uncompressed_length"
             << "unhex" << "unix_timestamp" << "updatexml" << "upper" << "user"
             << "utc_date" << "utc_time" << "utc_timestamp" << "uuid" << "uuid_short"
             << "uuid_to_bin" << "validate_password_strength" << "var_pop" << "var_samp"
             << "variance" << "version" << "wait_for_executed_gtid_set" << "wait_until_sql_thread_after_gtids" << "week"
             << "weekday" << "weekofyear" << "weight_string" << "year" << "yearweek";
    } else if (mDriverName == DRIVER_PSQL) {
        /*

https://www.postgresql.org/docs/11/functions-string.html
https://www.postgresql.org/docs/11/functions-binarystring.html
https://www.postgresql.org/docs/11/functions-json.html
https://www.postgresql.org/docs/11/functions-datetime.html
https://www.postgresql.org/docs/11/functions-array.html
https://www.postgresql.org/docs/11/functions-geometry.html

let ids = ['#FUNCTIONS-JSON-CREATION-TABLE tbody','#FUNCTIONS-JSON-PROCESSING-TABLE tbody',
    '#FUNCTIONS-STRING-SQL tbody','#FUNCTIONS-STRING-OTHER tbody','#FUNCTIONS-BINARYSTRING-SQL tbody',
    '#FUNCTIONS-BINARYSTRING-OTHER tbody','#ARRAY-FUNCTIONS-TABLE tbody','#FUNCTIONS-GEOMETRY-FUNC-TABLE tbody',
    '#FUNCTIONS-GEOMETRY-CONV-TABLE tbody']
let tables = ids.reduce( (p,c) => {let t = document.querySelector(c); if (t) p.push(t); return p} , [])

let tds = []
tables.forEach( t => [...t.querySelectorAll('tr')].forEach( tr => tr.querySelector('td').innerText.split(/\n|( or )/).forEach( e => tds.push(e) )))
tds = tds.map( e => {let m = e.match(/([^ ]+)[(]/); if (m) return m[1]} ).filter( e => e )
tds = Array.from(new Set(tds))
document.body.innerText = tds.map( (e,i) => ` << "${e}"` + ((i % 5) == 4 ? "\n" : "") ).join('')

https://www.postgresql.org/docs/11/functions-math.html

let ids = ['#FUNCTIONS-MATH-FUNC-TABLE tbody','#FUNCTIONS-MATH-TRIG-TABLE tbody']
let tables = ids.reduce( (p,c) => {let t = document.querySelector(c); if (t) p.push(t); return p} , [])
let tds = []
tables.forEach( t => [...t.querySelectorAll('tr')].forEach( tr => [...tr.querySelectorAll('td')].forEach(td => td.innerText.split(/\n|( or )/).forEach( e => tds.push(e) ))))
tds = tds.map( e => {let m = e.match(/([^ ]+)[(]/); if (m) return m[1]} ).filter( e => e )
tds = Array.from(new Set(tds))
document.body.innerText = tds.map( (e,i) => ` << "${e}"` + ((i % 7) == 6 ? "\n" : "") ).join('')

*/

        res << "bit_length" << "char_length" << "character_length" << "lower" << "octet_length"
            << "overlay" << "position" << "substring" << "substring" << "substring"
            << "trim" << "trim" << "upper" << "ascii" << "btrim"
            << "chr" << "concat" << "concat_ws" << "convert" << "convert_from"
            << "convert_to" << "decode" << "encode" << "format" << "initcap"
            << "left" << "length" << "length" << "lpad" << "ltrim"
            << "md5" << "parse_ident" << "pg_client_encoding" << "quote_ident" << "quote_literal"
            << "quote_literal" << "quote_nullable" << "quote_nullable" << "regexp_match" << "regexp_matches"
            << "regexp_replace" << "regexp_split_to_array" << "regexp_split_to_table" << "repeat" << "replace"
            << "reverse" << "right" << "rpad" << "rtrim" << "split_part"
            << "strpos" << "substr" << "starts_with" << "to_ascii" << "to_hex"
            << "translate"

            << "octet_length" << "overlay" << "position" << "substring" << "trim"
            << "btrim" << "decode" << "encode" << "get_bit" << "get_byte"
            << "length" << "md5" << "set_bit" << "set_byte" << "sha224"
            << "sha256" << "sha384" << "sha512"

            << "array_append" << "array_cat" << "array_ndims" << "array_dims" << "array_fill"
            << "array_length" << "array_lower" << "array_position" << "array_positions" << "array_prepend"
            << "array_remove" << "array_replace" << "array_to_string" << "array_upper" << "cardinality"
            << "string_to_array" << "unnest" << "unnest"

            << "to_json" << "to_jsonb" << "array_to_json" << "row_to_json" << "json_build_array"
            << "jsonb_build_array" << "json_build_object" << "jsonb_build_object" << "json_object" << "jsonb_object"
            << "json_object" << "jsonb_object" << "json_array_length" << "jsonb_array_length" << "json_each"
            << "jsonb_each" << "json_each_text" << "jsonb_each_text" << "json_extract_path" << "jsonb_extract_path"
            << "json_extract_path_text" << "jsonb_extract_path_text" << "json_object_keys" << "jsonb_object_keys" << "json_populate_record"
            << "jsonb_populate_record" << "json_populate_recordset" << "jsonb_populate_recordset" << "json_array_elements" << "jsonb_array_elements"
            << "json_array_elements_text" << "jsonb_array_elements_text" << "json_typeof" << "jsonb_typeof" << "json_to_record"
            << "jsonb_to_record" << "json_to_recordset" << "jsonb_to_recordset" << "json_strip_nulls" << "jsonb_strip_nulls"
            << "jsonb_set" << "jsonb_insert" << "jsonb_pretty"

            << "age" << "age" << "clock_timestamp" << "date_part" << "date_part"
            << "date_trunc" << "date_trunc" << "extract" << "extract" << "isfinite"
            << "isfinite" << "isfinite" << "justify_days" << "justify_hours" << "justify_interval"
            << "make_date" << "make_interval" << "make_time" << "make_timestamp" << "make_timestamptz"
            << "now" << "statement_timestamp" << "timeofday" << "transaction_timestamp" << "to_timestamp"

            << "area" << "center" << "diameter" << "height" << "isclosed"
            << "isopen" << "length" << "npoints" << "pclose" << "popen"
            << "radius" << "width" << "box" << "bound_box" << "circle"
            << "line" << "lseg" << "path" << "point" << "polygon"

            << "abs" << "cbrt" << "ceil" << "ceiling" << "degrees" << "div" << "exp"
            << "floor" << "ln" << "log" << "mod" << "pi" << "power" << "radians"
            << "round" << "scale" << "sign" << "sqrt" << "trunc" << "width_bucket" << "width_bucket(now"
            << "acos" << "acosd" << "asin" << "asind" << "atan" << "atand" << "atan2"
            << "atan2d" << "cos" << "cosd" << "cot" << "cotd" << "sin" << "sind"
            << "tan" << "tand";
    }
    return res;
}


QStringList Tokens::keywords() const
{
    QStringList res;
    res << commonKeywords();
    res << driverKeywords();
    return res;
}

QStringList Tokens::tablesAndFields(bool doted) const {
    QStringList res;
    foreach(const Table& table, mTables) {
        res << table.table;
        foreach(const QString& field, table.fields) {
            res << field;
            if (doted) {
                res << (table.table + "." + field);
            }
        }
    }
    return res;
}

QStringList Tokens::fields(const QString& tableName, const QString& alias) const {
    QStringList res;
    foreach(const Table& table, mTables) {
        if (table.table != tableName) {
            continue;
        }
        foreach(const QString& field, table.fields) {
            res << (alias + "." + field);
        }
    }
    return res;
}

QStringList Tokens::fields(const QString &tableName, bool dotted) const
{
    QStringList res;
    foreach(const Table& table, mTables) {
        if (table.table != tableName) {
            continue;
        }
        if (dotted) {
            foreach(const QString& field, table.fields) {
                res << (table.table + "." + field);
            }
        } else {
            foreach(const QString& field, table.fields) {
                res << field;
            }
        }
    }
    return res;
}

QStringList Tokens::fields(bool doted) const
{
    QStringList res;
    foreach(const Table& table, mTables) {
        foreach(const QString& field, table.fields) {
            if (doted) {
                res << (table.table + "." + field);
            } else {
                res << field;
            }
        }
    }
    return res;
}

QStringList Tokens::tables() const
{
    QStringList res;
    foreach(const Table& table, mTables) {
        res << table.table;
    }
    return res;
}

static QStringList uniq(const QStringList& values) {
    QStringList res;
    for(const QString& value: values) {
        if (!res.contains(value)) {
            res.append(value);
        }
    }
    return res;
}


QStringList Tokens::autocompletion(const QMap<QString,QString>& aliases) const
{
    QStringList res;
    res << keywords();
    res << tablesAndFields(true);
    res << functions();
    res << types();

    QStringList names = aliases.keys();
    foreach (const QString& name, names) {
        res << fields(aliases[name],name);
        res << name;
    }

    res = uniq(res);
    std::sort(res.begin(), res.end());
    return res;
}

QStringList Tokens::types() const
{
    QStringList res;
    if (mDriverName == DRIVER_MYSQL) {
        res << "integer" << "smallint" << "decimal" << "numeric" << "float"
            << "real" << "double" << "int" << "dec" << "fixed"
            << "double precision" << "date" << "datetime" << "time"
            << "timestamp" << "year" << "char" << "varchar" << "binary"
            << "varbinary" << "blob" << "text" << "enum";
    } else if (mDriverName == DRIVER_PSQL) {
/*

https://www.postgresql.org/docs/11/datatype-numeric.html
https://www.postgresql.org/docs/11/datatype-money.html
https://www.postgresql.org/docs/11/datatype-character.html
https://www.postgresql.org/docs/11/datatype-boolean.html
https://www.postgresql.org/docs/11/datatype-geometric.html
https://www.postgresql.org/docs/11/datatype-net-types.html
https://www.postgresql.org/docs/11/datatype-json.html

let ids = ['#DATATYPE-NUMERIC-TABLE tbody','#DATATYPE-MONEY-TABLE tbody',
    '#DATATYPE-BINARY-TABLE tbody','#DATATYPE-BOOLEAN-TABLE tbody','#DATATYPE-GEO-TABLE tbody',
    '#DATATYPE-NET-TYPES-TABLE tbody','#JSON-TYPE-MAPPING-TABLE tbody']
let tables = ids.reduce( (p,c) => {let t = document.querySelector(c); if (t) p.push(t); return p} , [])
let tds = []
tables.forEach( t => [...t.querySelectorAll('tr')].forEach( tr => tds.push(tr.querySelector('td').innerText) ))
tds = Array.from(new Set(tds))
document.body.innerText = tds.map( (e,i) => ` << "${e}"` + ((i % 7) == 6 ? "\n" : "") ).join('')

https://www.postgresql.org/docs/11/datatype-character.html
https://www.postgresql.org/docs/11/datatype-datetime.html

let ids = ['#DATATYPE-CHARACTER-TABLE tbody','#DATATYPE-DATETIME-TABLE tbody']
let tables = ids.reduce( (p,c) => {let t = document.querySelector(c); if (t) p.push(t); return p} , [])
let tds = []
tables.forEach( t => [...t.querySelectorAll('tr')].forEach( tr => tr.querySelector('td').innerText.split(',').forEach( e => tds.push(e) )))
tds = tds.map( td => td.split(/[(\[]/)[0].trim() )
tds = Array.from(new Set(tds))
document.body.innerText = tds.map( (e,i) => ` << "${e}"` + ((i % 7) == 6 ? "\n" : "") ).join('')
*/
        res << "smallint" << "integer" << "bigint" << "decimal" << "numeric" << "real" << "double precision"
            << "smallserial" << "serial" << "bigserial"
            << "money"
            << "character varying" << " varchar" << "character" << " char" << "text"
            << "bytea"
            << "timestamp" << "date" << "time" << "interval"
            << "boolean"
            << "point" << "line" << "lseg" << "box" << "path" << "polygon" << "circle"
            << "cidr" << "inet" << "macaddr" << "macaddr8"
            << "tsvector" << "tsquery" << "uuid" << "xml"
            << "string" << "number" << "boolean" << "null"
            << "json" << "jsonb";

    }
    return res;
}

QStringList Tokens::driverKeywords() const
{
    QStringList res;
    if (mDriverName == DRIVER_MYSQL) {

        /*
            https://dev.mysql.com/doc/refman/8.0/en/select.html
            https://dev.mysql.com/doc/refman/8.0/en/join.html
            https://dev.mysql.com/doc/refman/8.0/en/alter-table.html
            https://dev.mysql.com/doc/refman/8.0/en/insert.html
            https://dev.mysql.com/doc/refman/8.0/en/show.html

            let lists = [],
                list = [],
                addToList = (t) => list.push(t),
                flushList = () => {
                    if (list.length > 0) {
                        lists.push(list);
                        list = [];
                    }};
            [...document.querySelector('code.language-sql').childNodes]
                .forEach(node => {
                    if (node.textContent.match(/^\s+$/)) {

                    } else if (node.tagName == 'SPAN' && node.classList.contains('keyword')) {
                        addToList(node.innerText);
                    } else {
                        flushList();
                    }
                })
            flushList();
            let keywords = lists.map( list => list.join(' ').toLowerCase() );
            document.body.innerText = keywords.map( (e,i) => ` << "${e}"` + ((i % 8) == 7 ? "\n" : "") ).join('');

*/

        res  << "select" << "all" << "distinct" << "distinctrow" << "high_priority" << "straight_join" << "sql_small_result" << "sql_big_result"
             << "sql_buffer_result" << "sql_no_cache" << "sql_calc_found_rows" << "from" << "partition" << "where" << "group by" << "with rollup"
             << "having" << "window" << "as" << "as" << "order by" << "asc" << "desc" << "with rollup"
             << "limit" << "offset" << "row_count" << "row_count offset offset" << "into outfile" << "character set" << "into dumpfile" << "into"
             << "for" << "update" << "share" << "of" << "nowait" << "skip locked" << "lock in share mode"

             << "partition" << "as" << "as" << "inner" << "cross" << "join" << "straight_join" << "straight_join"
             << "on" << "left" << "right" << "outer" << "join" << "natural" << "inner" << "left"
             << "right" << "outer" << "join" << "on" << "using" << "use" << "index" << "key"
             << "for" << "join" << "order by" << "group by" << "ignore" << "index" << "key" << "for"
             << "join" << "order by" << "group by" << "force" << "index" << "key" << "for" << "join"
             << "order by" << "group by"

             << "alter table" << "add" << "column" << "first" << "after" << "add" << "column" << "add"
             << "index" << "key" << "add" << "constraint" << "primary key" << "add" << "constraint" << "unique"
             << "index" << "key" << "add fulltext" << "index" << "key" << "add spatial" << "index" << "key"
             << "add" << "constraint" << "foreign key" << "algorithm" << "default" << "alter" << "column" << "set default"
             << "drop default" << "alter index" << "visible" << "invisible" << "change" << "column" << "first" << "after"
             << "default" << "character set" << "collate" << "to character set" << "collate" << "disable" << "enable" << "keys"
             << "discard" << "import" << "tablespace" << "drop" << "column" << "drop" << "index" << "key"
             << "drop primary key" << "drop foreign key" << "force" << "lock" << "default" << "none" << "modify" << "column"
             << "first" << "after" << "order by" << "rename column" << "to" << "rename" << "index" << "key"
             << "to" << "rename" << "to" << "as" << "without" << "with" << "validation" << "add partition"
             << "drop partition" << "discard partition" << "all" << "tablespace" << "import partition" << "all" << "tablespace" << "truncate partition"
             << "all" << "coalesce partition number" << "reorganize partition" << "into" << "exchange partition" << "with table" << "with" << "without"
             << "validation" << "analyze partition" << "all" << "check partition" << "all" << "optimize partition" << "all" << "rebuild partition"
             << "all" << "repair partition" << "all" << "remove partitioning" << "upgrade partitioning" << "asc" << "desc" << "using"
             << "btree" << "hash" << "key_block_size" << "value" << "with parser" << "comment" << "visible" << "invisible"
             << "auto_increment" << "value" << "avg_row_length" << "value" << "default" << "character set" << "checksum" << "default"
             << "collate" << "comment" << "compression" << "connection" << "data" << "index" << "directory" << "delay_key_write"
             << "encryption" << "engine" << "insert_method" << "no" << "first" << "last" << "key_block_size" << "value"
             << "max_rows" << "value" << "min_rows" << "value" << "pack_keys" << "default" << "password" << "row_format"
             << "default" << "dynamic" << "fixed" << "compressed" << "redundant" << "compact" << "stats_auto_recalc" << "default"
             << "stats_persistent" << "default" << "stats_sample_pages" << "value" << "tablespace" << "storage" << "disk" << "memory"
             << "default" << "union" << "create table"

             << "insert" << "low_priority" << "delayed" << "high_priority" << "ignore" << "into" << "partition"
             << "value" << "on duplicate key update" << "insert" << "low_priority" << "delayed" << "high_priority" << "ignore" << "into"
             << "partition" << "set" << "on duplicate key update" << "insert" << "low_priority" << "high_priority" << "ignore" << "into"
             << "partition" << "select" << "on duplicate key update" << "value" << "default" << "value" << "value" << "value"

             << "show binary logs" << "show binlog events" << "show character set"
             << "show collation" << "show columns" << "show create database"
             << "show create event" << "show create function"
             << "show create procedure" << "show create table"
             << "show create trigger" << "show create user" << "show create view"
             << "show databases" << "show engine" << "show engines" << "show errors"
             << "show events" << "show function code" << "show function status"
             << "show grants" << "show index" << "show master status"
             << "show open tables" << "show plugins" << "show privileges"
             << "show procedure code" << "show procedure status"
             << "show processlist" << "show profile" << "show profiles"
             << "show relaylog events" << "show slave hosts" << "show slave status"
             << "show status" << "show table status" << "show tables"
             << "show triggers" << "show variables" << "show warnings"

             << "references";
    } else if (mDriverName == DRIVER_PSQL) {
/*
https://www.postgresql.org/docs/9.1/sql-keywords-appendix.html
let keywords = [...document.querySelectorAll('.CALSTABLE tbody')].slice(0,2).map( tbody => [...tbody.querySelectorAll('tr')].map( e => e.querySelector('td').innerText ) ).reduce( (p,c) => p.concat(c) ).map( e => e.toLowerCase() )
document.body.innerText = keywords.map( (e,i) => ` << "${e}"` + ((i % 5) == 4 ? "\n" : "") ).join('');
*/

        res << "a" << "abort" << "abs" << "absent" << "absolute"
            << "access" << "according" << "action" << "ada" << "add"
            << "admin" << "after" << "aggregate" << "alias" << "all"
            << "allocate" << "also" << "alter" << "always" << "analyse"
            << "analyze" << "and" << "any" << "are" << "array"
            << "array_agg" << "as" << "asc" << "asensitive" << "assertion"
            << "assignment" << "asymmetric" << "at" << "atomic" << "attribute"
            << "attributes" << "authorization" << "avg" << "backward" << "base64"
            << "before" << "begin" << "bernoulli" << "between" << "bigint"
            << "binary" << "bit" << "bitvar" << "bit_length" << "blob"
            << "blocked" << "bom" << "boolean" << "both" << "breadth"
            << "by" << "c" << "cache" << "call" << "called"
            << "cardinality" << "cascade" << "cascaded" << "case" << "cast"
            << "catalog" << "catalog_name" << "ceil" << "ceiling" << "chain"
            << "char" << "character" << "characteristics" << "characters" << "character_length"
            << "character_set_catalog" << "character_set_name" << "character_set_schema" << "char_length" << "check"
            << "checked" << "checkpoint" << "class" << "class_origin" << "clob"
            << "close" << "cluster" << "coalesce" << "cobol" << "collate"
            << "collation" << "collation_catalog" << "collation_name" << "collation_schema" << "collect"
            << "column" << "columns" << "column_name" << "command_function" << "command_function_code"
            << "comment" << "comments" << "commit" << "committed" << "completion"
            << "concurrently" << "condition" << "condition_number" << "configuration" << "connect"
            << "connection" << "connection_name" << "constraint" << "constraints" << "constraint_catalog"
            << "constraint_name" << "constraint_schema" << "constructor" << "contains" << "content"
            << "continue" << "control" << "conversion" << "convert" << "copy"
            << "corr" << "corresponding" << "cost" << "count" << "covar_pop"
            << "covar_samp" << "create" << "cross" << "csv" << "cube"
            << "cume_dist" << "current" << "current_catalog" << "current_date" << "current_default_transform_group"
            << "current_path" << "current_role" << "current_schema" << "current_time" << "current_timestamp"
            << "current_transform_group_for_type" << "current_user" << "cursor" << "cursor_name" << "cycle"
            << "data" << "database" << "datalink" << "date" << "datetime_interval_code"
            << "datetime_interval_precision" << "day" << "db" << "deallocate" << "dec"
            << "decimal" << "declare" << "default" << "defaults" << "deferrable"
            << "deferred" << "defined" << "definer" << "degree" << "delete"
            << "delimiter" << "delimiters" << "dense_rank" << "depth" << "deref"
            << "derived" << "desc" << "describe" << "descriptor" << "destroy"
            << "destructor" << "deterministic" << "diagnostics" << "dictionary" << "disable"
            << "discard" << "disconnect" << "dispatch" << "distinct" << "dlnewcopy"
            << "dlpreviouscopy" << "dlurlcomplete" << "dlurlcompleteonly" << "dlurlcompletewrite" << "dlurlpath"
            << "dlurlpathonly" << "dlurlpathwrite" << "dlurlscheme" << "dlurlserver" << "dlvalue"
            << "do" << "document" << "domain" << "double" << "drop"
            << "dynamic" << "dynamic_function" << "dynamic_function_code" << "each" << "element"
            << "else" << "empty" << "enable" << "encoding" << "encrypted"
            << "end" << "end-exec" << "enum" << "equals" << "escape"
            << "every" << "except" << "exception" << "exclude" << "excluding"
            << "exclusive" << "exec" << "execute" << "existing" << "exists"
            << "exp" << "explain" << "extension" << "external" << "extract"
            << "false" << "family" << "fetch" << "file" << "filter"
            << "final" << "first" << "first_value" << "flag" << "float"
            << "floor" << "following" << "for" << "force" << "foreign"
            << "fortran" << "forward" << "found" << "free" << "freeze"
            << "from" << "fs" << "full" << "function" << "functions"
            << "fusion" << "g" << "general" << "generated" << "get"
            << "global" << "go" << "goto" << "grant" << "granted"
            << "greatest" << "group" << "grouping" << "handler" << "having"
            << "header" << "hex" << "hierarchy" << "hold" << "host"
            << "hour" << "id" << "identity" << "if" << "ignore"
            << "ilike" << "immediate" << "immutable" << "implementation" << "implicit"
            << "import" << "in" << "including" << "increment" << "indent"
            << "index" << "indexes" << "indicator" << "infix" << "inherit"
            << "inherits" << "initialize" << "initially" << "inline" << "inner"
            << "inout" << "input" << "insensitive" << "insert" << "instance"
            << "instantiable" << "instead" << "int" << "integer" << "integrity"
            << "intersect" << "intersection" << "interval" << "into" << "invoker"
            << "is" << "isnull" << "isolation" << "iterate" << "join"
            << "k" << "key" << "key_member" << "key_type" << "label"
            << "lag" << "language" << "large" << "last" << "last_value"
            << "lateral" << "lc_collate" << "lc_ctype" << "lead" << "leading"
            << "least" << "left" << "length" << "less" << "level"
            << "library" << "like" << "like_regex" << "limit" << "link"
            << "listen" << "ln" << "load" << "local" << "localtime"
            << "localtimestamp" << "location" << "locator" << "lock" << "lower"
            << "m" << "map" << "mapping" << "match" << "matched"
            << "max" << "maxvalue" << "max_cardinality" << "member" << "merge"
            << "message_length" << "message_octet_length" << "message_text" << "method" << "min"
            << "minute" << "minvalue" << "mod" << "mode" << "modifies"
            << "modify" << "module" << "month" << "more" << "move"
            << "multiset" << "mumps" << "name" << "names" << "namespace"
            << "national" << "natural" << "nchar" << "nclob" << "nesting"
            << "new" << "next" << "nfc" << "nfd" << "nfkc"
            << "nfkd" << "nil" << "no" << "none" << "normalize"
            << "normalized" << "not" << "nothing" << "notify" << "notnull"
            << "nowait" << "nth_value" << "ntile" << "null" << "nullable"
            << "nullif" << "nulls" << "number" << "numeric" << "object"
            << "occurrences_regex" << "octets" << "octet_length" << "of" << "off"
            << "offset" << "oids" << "old" << "on" << "only"
            << "open" << "operation" << "operator" << "option" << "options"
            << "or" << "order" << "ordering" << "ordinality" << "others"
            << "out" << "outer" << "output" << "over" << "overlaps"
            << "overlay" << "overriding" << "owned" << "owner" << "p"
            << "pad" << "parameter" << "parameters" << "parameter_mode" << "parameter_name"
            << "parameter_ordinal_position" << "parameter_specific_catalog" << "parameter_specific_name" << "parameter_specific_schema" << "parser"
            << "partial" << "partition" << "pascal" << "passing" << "passthrough"
            << "password" << "path" << "percentile_cont" << "percentile_disc" << "percent_rank"
            << "permission" << "placing" << "plans" << "pli" << "position"
            << "position_regex" << "postfix" << "power" << "preceding" << "precision"
            << "prefix" << "preorder" << "prepare" << "prepared" << "preserve"
            << "primary" << "prior" << "privileges" << "procedural" << "procedure"
            << "public" << "quote" << "range" << "rank" << "read"
            << "reads" << "real" << "reassign" << "recheck" << "recovery"
            << "recursive" << "ref" << "references" << "referencing" << "regr_avgx"
            << "regr_avgy" << "regr_count" << "regr_intercept" << "regr_r2" << "regr_slope"
            << "regr_sxx" << "regr_sxy" << "regr_syy" << "reindex" << "relative"
            << "release" << "rename" << "repeatable" << "replace" << "replica"
            << "requiring" << "reset" << "respect" << "restart" << "restore"
            << "restrict" << "result" << "return" << "returned_cardinality" << "returned_length"
            << "returned_octet_length" << "returned_sqlstate" << "returning" << "returns" << "revoke"
            << "right" << "role" << "rollback" << "rollup" << "routine"
            << "routine_catalog" << "routine_name" << "routine_schema" << "row" << "rows"
            << "row_count" << "row_number" << "rule" << "savepoint" << "scale"
            << "schema" << "schema_name" << "scope" << "scope_catalog" << "scope_name"
            << "scope_schema" << "scroll" << "search" << "second" << "section"
            << "security" << "select" << "selective" << "self" << "sensitive"
            << "sequence" << "sequences" << "serializable" << "server" << "server_name"
            << "session" << "session_user" << "set" << "setof" << "sets"
            << "share" << "show" << "similar" << "simple" << "size"
            << "smallint" << "some" << "source" << "space" << "specific"
            << "specifictype" << "specific_name" << "sql" << "sqlcode" << "sqlerror"
            << "sqlexception" << "sqlstate" << "sqlwarning" << "sqrt" << "stable"
            << "standalone" << "start" << "state" << "statement" << "static"
            << "statistics" << "stddev_pop" << "stddev_samp" << "stdin" << "stdout"
            << "storage" << "strict" << "strip" << "structure" << "style"
            << "subclass_origin" << "sublist" << "submultiset" << "substring" << "substring_regex"
            << "sum" << "symmetric" << "sysid" << "system" << "system_user"
            << "t" << "table" << "tables" << "tablesample" << "tablespace"
            << "table_name" << "temp" << "template" << "temporary" << "terminate"
            << "text" << "than" << "then" << "ties" << "time"
            << "timestamp" << "timezone_hour" << "timezone_minute" << "to" << "token"
            << "top_level_count" << "trailing" << "transaction" << "transactions_committed" << "transactions_rolled_back"
            << "transaction_active" << "transform" << "transforms" << "translate" << "translate_regex"
            << "translation" << "treat" << "trigger" << "trigger_catalog" << "trigger_name"
            << "trigger_schema" << "trim" << "trim_array" << "true" << "truncate"
            << "trusted" << "type" << "uescape" << "unbounded" << "uncommitted"
            << "under" << "unencrypted" << "union" << "unique" << "unknown"
            << "unlink" << "unlisten" << "unlogged" << "unnamed" << "unnest"
            << "until" << "untyped" << "update" << "upper" << "uri"
            << "usage" << "user" << "user_defined_type_catalog" << "user_defined_type_code" << "user_defined_type_name"
            << "user_defined_type_schema" << "using" << "vacuum" << "valid" << "validate"
            << "validator" << "value" << "values" << "varbinary" << "varchar"
            << "variable" << "variadic" << "varying" << "var_pop" << "var_samp"
            << "verbose" << "version" << "view" << "volatile" << "when"
            << "whenever" << "where" << "whitespace" << "width_bucket" << "window"
            << "with" << "within" << "without" << "work" << "wrapper"
            << "write" << "xml" << "xmlagg" << "xmlattributes" << "xmlbinary"
            << "xmlcast" << "xmlcomment" << "xmlconcat" << "xmldeclaration" << "xmldocument"
            << "xmlelement" << "xmlexists" << "xmlforest" << "xmliterate" << "xmlnamespaces"
            << "xmlparse" << "xmlpi" << "xmlquery" << "xmlroot" << "xmlschema"
            << "xmlserialize" << "xmltable" << "xmltext" << "xmlvalidate" << "year"
            << "yes" << "zone";

    }

    return res;
}

QStringList Tokens::commonKeywords() const{
    QStringList res;
    res << "select" << "from" << "join" << "left join" << "right join" << "inner join"
          << "insert" << "into" << "values" << "delete" << "update" << "set" << "truncate"
          << "union" << "create table" << "create view" << "create" << "where" << "having"
          << "not" << "and" << "group by" << "order by" << "asc" << "desc" << "top"
          << "limit" << "offset" << "on" << "like" << "or" << "case" << "else" << "in"
          << "is" << "null";
    return res;
}
