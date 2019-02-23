function check_js_version()
{
    var _is_support = true;
    try
    {
        let _i = 0;
        const _j = 0;
        [].forEach(function ()
        {

        })
    } catch (e)
    {
        _is_support = false;
    }
    return _is_support;
}
if (check_js_version())
{
    $.ajax({
        url: "js/source/storage.js",
        dataType: "script",
        async: false
    });
    $.ajax({
        url: "js/source/deffunc.js",
        dataType: "script",
        async: false
    });
    $.ajax({
        url: "js/source/data_parser.js",
        dataType: "script",
        async: false
    });
    $.ajax({
        url: "js/source/data_presentation.js",
        dataType: "script",
        async: false
    });
    $.ajax({
        url: "js/source/info.js",
        dataType: "script",
        async: false
    });
    $.ajax({
        url: "js/source/sniffer.js",
        dataType: "script",
        async: false
    });
    $.ajax({
        url: "js/source/net.js",
        dataType: "script",
        async: false
    });
    $.ajax({
        url: "js/source/main.js",
        dataType: "script",
        async: false
    });
} else
{
    $(document).ready(function ()
    {
        alert("Your browser does not support ECMAScript 6. Please upgrade your browser!");
    })
}


