/**
 * Created by s.n.cherepanov on 01.03.2017.
 */
function check_js_version ()
{
    var _is_support=true;
    try
    {
        let _i=0;
        const _j=0;
        [].forEach(function ()
        {

        })
    }catch(e)
    {
        _is_support=false;
    }
    return _is_support;
}
if(check_js_version())
{
    $.getScript("js/source/storage.js");
    $.getScript("js/source/deffunc.js");
    $.getScript("js/source/main.js");
    $.getScript("js/source/data_parser.js");
    $.getScript("js/source/data_presentation.js");
    $.getScript("js/source/info.js");
    $.getScript("js/source/sniffer.js");
}else
{
    $(document).ready(function ()
    {
        alert("Your browser does not support ECMAScript 6. Please upgrade your browser!");
    })
}


