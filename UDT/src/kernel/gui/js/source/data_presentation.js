function get_uuids_class()
{
    return "." + get_id_for("class-" + data_info.uuid.getName()) + ",." + get_id_for("class-Registered_By")
        + ",." + get_id_for("class-To");
}
function filling_data_style(_f)
{
    _f.tooltip({//for uuid
            content: function ()
            {
                let _uuid = $(this).text();
                let _info = storage.susbscriber_net.get(_uuid);
                let _error;
                if ($(this).hasClass("fail-send"))
                {
                    _error = $(this).attr("error_type");
                }
                ;
                if (_info)
                {
                    let _to = $("<table>");
                    if (_error)
                    {
                        let _rep = $.to_representation_form({
                            ecode: _error
                        }, data_info);
                        $.object_to_table(_rep, _to);
                        $("tr",_to).addClass("ui-state-error");
                    }

                    let _rep = $.to_representation_form({info: _info}, data_info);
                    if (_rep.hasOwnProperty("is_removed"))
                        _to.addClass("ui-state-error");
                    $.object_to_table(_rep, _to, true);

                    return _to;
                } else if (_error)
                {
                    let _to = $("<table>");
                    let _rep = $.to_representation_form({
                        ecode: _error
                    }, data_info);
                    $.object_to_table(_rep, _to);
                    $("tr",_to).addClass("ui-state-error");
                    $("<td>").attr("colspan", 2).text("Additional info is not exist").appendTo($("<tr>").addClass("ui-state-default").appendTo(_to))
                    return _to;
                }
                else
                {
                    return $("<span>").addClass("ui-state-error").text("Additional info is not exist");
                }
            },
            items: get_uuids_class(),
            show: {
                effect: "",
                delay: 1000
            }
        }
    );
}
var data_presentation =
    {}