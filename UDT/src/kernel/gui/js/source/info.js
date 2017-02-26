/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

function udt_info(aPanel)
{
    $.ajax(
            {
                type: "GET",
                data: {query: ["desc","info","res"]},
                dataType: "json"
            }).done(function (data)
    {
        var _table=$("<table>");
        //fill_table(data.state, _table);
        var _data=$.to_representation_form(data.state, data_info);
        $.object_to_table(_data, _table);
        $("table",_table).each(function()
        {
            $(this).attr("border","1");
        });
        _table.appendTo(aPanel);
    });
}
;
