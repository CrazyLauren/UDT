function filling_data_style(_f)
{
    _f.tooltip({//for uuid
            content: function ()
            {
                let _uuid=$(this).text();
                let _info = storage.susbscriber_net.get(_uuid);
                if (_info)
                {
                    let _rep = $.to_representation_form({info: _info}, data_info);
                    let _to = $("<table>");
                    if (_rep.hasOwnProperty("is_removed"))
                        _to.addClass("ui-state-error");
                    $.object_to_table(_rep[data_info.info.getName()], _to);
                    return _to;
                } else
                {
                    return $("<span>").addClass("ui-state-error").text("No info");
                }
            },
            items: ".uuid-tooltip,."+get_id_for("class-"+data_info.uuid.getName())+",."+get_id_for("class-Registered_By")
                        +",."+get_id_for("class-To"),
            show: {
                effect: "",
                delay:1000
            }
        }
    );
}
var data_presentation =
    {

    }