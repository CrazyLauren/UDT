function udt_info(aPanel)
{
    $.ajax(
            {
                type: "GET",
                data: {query: ["desc","info","modules"]},
                dataType: "json"
            }).done(function (data)
    {
        let _table=$("<table>");
        //fill_table(data.state, _table);
        let _data=$.to_representation_form(data.state, data_info);
        $.object_to_table(_data, _table);
        filling_data_style(_table);
        $("table",_table).each(function()
        {
            $(this).attr("border","1");
        });
        _table.appendTo(aPanel);
    });
}
;
