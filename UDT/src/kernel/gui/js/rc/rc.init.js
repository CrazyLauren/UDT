/**
 * Created by s.n.cherepanov on 01.03.2017.
 */
function check_js_version ()
{
    this._is_support=false;
    var d = document;
    var g = d.createElement('script'),
        s = d.getElementsByTagName('script')[0];

    g.setAttribute("language", "JavaScript1.7");
    g.text = "this._is_support=true;";
    s.parentNode.insertBefore(g, s);
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
        alert("Your browser does not support ECMAScript 6");
    })
}


