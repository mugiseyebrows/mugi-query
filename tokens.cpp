#include "tokens.h"
#include "tokens.h"

#include <QSet>
#include <QSqlRecord>

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

Tokens::Tokens(QSqlDatabase db)
{
    QStringList tableNames = db.tables();
    foreach(const QString& tableName, tableNames) {
        Table table;
        table.table = tableName.toLower();
        table.fields = tableFields(db, tableName);
        mTables.append(table);
    }
    mDriverName = db.driverName();
}

QStringList Tokens::functions() const
{
    QStringList res;

    if (mDriverName == "QMYSQL") {

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

QStringList Tokens::autocompletion() const
{
    QStringList res;
    res << keywords();
    res << tablesAndFields(true);
    res << functions();
    res << types();
    res = res.toSet().toList();
    qSort(res);
    return res;
}

QStringList Tokens::types() const
{
    QStringList res;
    if (mDriverName == "QMYSQL") {
        res << "integer" << "smallint" << "decimal" << "numeric" << "float"
            << "real" << "double" << "int" << "dec" << "fixed"
            << "double precision" << "date" << "datetime" << "time"
            << "timestamp" << "year" << "char" << "varchar" << "binary"
            << "varbinary" << "blob" << "text" << "enum";
    }
    return res;
}

QStringList Tokens::driverKeywords() const
{
    QStringList res;
    if (mDriverName == "QMYSQL") {

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
             << "show triggers" << "show variables" << "show warnings";
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
