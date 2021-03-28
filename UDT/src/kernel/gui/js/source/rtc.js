var rtc =//static pane
    {
        MAIN_REGION: "rtc_region",
        RTC_DATA: "rtc_data_region",
        BUTTONS: "rtc_buttons_region",
        // DATA_OF_TAB: "tab_data",
        // CLASS_CHECK_BOX: "handler_check_box",
        // CLASS_TAB_OF_FROM: "tab_of_from",
        // CLASS_RAW_COLOR: "handler-class-",
        FContainer: null,
        open: function (aPanel)
        {
            let self = rtc;

            self.get_or_create_pane(aPanel);
            self.refresh();
        },
        create_control_buttons: function (aTo)
        {
            let self = rtc;

            let _button_refresh = $("<button>").button();
            _button_refresh.attr("id", "rtc_refresh");
            _button_refresh.on("click", self.refresh);
            _button_refresh.text("Refresh");
            _button_refresh.appendTo(aTo);
        },
        get_or_create_pane: function (aPanel)
        {
            let self = rtc;

            if (self.FContainer)
                return self.FContainer;

            self.FContainer = $("<div>").attr("id", "rtc_pane").appendTo(aPanel);

            let _buttons = $("<div>").attr({
                id: rtc.BUTTONS
            }).appendTo(self.FContainer);
            self.create_control_buttons(_buttons);//todo object

            let _main = $("<div>").attr("id", self.MAIN_REGION).appendTo(self.FContainer);

            let _rtc_table = $("<table>").addClass("data_table").attr({
                    id: rtc.RTC_DATA
                }).appendTo(_main);

            let _thead = $("<thead>").appendTo(_rtc_table);
            let _tr = $("<tr>").appendTo(_thead);
            $("<th>").text("Type").appendTo(_tr);
            $("<th>").text("Date").appendTo(_tr);
            $("<th>").text("Time (sec)").appendTo(_tr);
            $("<th>").text("Hardware time").appendTo(_tr);
            $("<th>").text("Jobs").appendTo(_tr);
            $("<th>").text("Owner").appendTo(_tr);
            $("<th>").text("ID").appendTo(_tr);

            $("<tbody>").appendTo(_rtc_table);

            storage.rtc.updated = rtc.update_data;
            return self.FContainer;
        },
        refresh: function ()
        {
            storage.rtc.update();
        },
        update_data: function (aData) {
            let self = rtc;
            let _table = $("#" + rtc.RTC_DATA);
            let tbody = $("tbody",_table);
            tbody.empty();

             aData.forEach(function (item)
                    {
                        const _id = item.time.owner + item.time.uid;
                        let _tr = $("<tr>").appendTo(tbody);
                        _tr.attr({
                            rtcId: _id
                        });
                        $("<td>").text(item.rtc_type).appendTo(_tr);
                        $("<td>").text(new Date(item.time.time/1000/1000)).appendTo(_tr);
                        $("<td>").text(item.time.time/1000/1000).appendTo(_tr);
                        $("<td>").text(item.time.time_hw).appendTo(_tr);
                        $("<td>").text(item.time.jobs).appendTo(_tr);
                        $("<td>").text(item.time.owner).appendTo(_tr);
                        $("<td>").text(item.time.uid).appendTo(_tr);
                    });
        }
    }

