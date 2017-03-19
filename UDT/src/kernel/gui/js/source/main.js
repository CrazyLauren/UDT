
$(document).ready(function ()
{
    document.title = "UDT";
    $.ajax(
        {
            type: "GET",
            data: {query:"info"},
            dataType: "json"
        });
    let _main = $("<div>").attr("id", "tabs").appendTo($("body"));
    let _ul = $("<ul>").appendTo(_main);

    function create_tab(aId, aName)
    {
        $("<a>").attr({
            href: '#' + aId
        }).text(aName).appendTo(
            $("<li>")
                .appendTo(_ul));
        $("<div>").attr({
            id: aId
        }).appendTo(_main);
    }

    //create_tab("tabs-router","Router");
    //create_tab("tabs-firewall","FireWall");
    create_tab("tabs-net","Net");
    //create_tab("tabs-send","Send");
    create_tab("tabs-sniffer","Sniffer");
    //create_tab("tabs-info","Info");
    //create_tab("tabs-settings","Settings");
    var handle_activation=function(tab)
    {
        let _tab_id = tab.attr("id");
        switch (_tab_id)
        {
            case "tabs-info":
                udt_info(tab.empty());
                break;
            case "tabs-sniffer":
                sniffer.open(tab);
                break;
            case "tabs-net":
                udt_net.open(tab);
                break;
            default:
                tab.empty().html("Couldn't load this tab");
                break;
        }
    };

     _main.tabs(
            {
                active: 0,
                collapsible: false,
                event: "click",
                create:function (event, ui)
                {
                    handle_activation(ui.panel);
                },
                activate: function (event, ui)
                {
                    handle_activation(ui.newPanel);
                }
            }
        );
    _main.tabs({active:0});

});

