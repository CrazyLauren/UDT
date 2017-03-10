var udt_net =//static pane
    {
        FContainer: null,
        FCy:null,
        open: function (aPanel)
        {
            let self = udt_net;
            self.get_or_create_pane(aPanel);
            storage.roads.updated=self.update;
            self.refresh();
        },
        get_or_create_pane: function (aPanel)
        {
            let self = udt_net;

            if (self.FContainer)
                return self.FContainer;

            self.FContainer = $("<div>").attr("id", "udt_net_pane").appendTo(aPanel);
            self.FCy=cytoscape({
                container: self.FContainer,
                layout: {
                    name: 'breadthfirst',
                    roots:storage.kernel_uuid
                },
                style: [
                    {
                        selector: 'node[type="1"]',
                        style: {
                            'label': 'data(name)',
                            'text-valign': 'center',
                            'width': 'label',
                            'height': 'auto',
                            'shape':'ellipse',
                            'background-color':"lightseagreen"
                        }
                    },
                    {
                        selector: 'node[type="2"]',
                        style: {
                            'label': 'data(name)',
                            'text-valign': 'center',
                            'width': 'label',
                            'height': 'auto',
                            'shape':'star',
                            'background-color':"cornflowerblue"
                        }
                    }
                ]
            });

            // storage.roads.on_new = self.update_data;
            // storage.roads.updated = self.update_data;

            return self.FContainer;
        },
        refresh: function ()
        {
            storage.roads.update();
        },
        update: function (aElements)
        {
            let self = udt_net;
            aElements.nodes.forEach(function (item)
            {
                item.info=storage.susbscriber_net.get(item.data.id,false);
                if(item.info)
                {
                    item.data.type=item.info.type;
                    item.data.name=item.info.id.n;
                }
            })

            //self.FCy.add(aElements);
            //self.FCy.reset();
            self.FCy.load(aElements);
        }
    }