/*!
 * /**
 * * @name JSON Editor
 * * @description JSON Schema Based Editor
 * * This library is the continuation of jdorn's great work (see also https://github.com/jdorn/json-editor/issues/800)
 * * @version "2.3.0"
 * * @author Jeremy Dorn
 * * @see https://github.com/jdorn/json-editor/
 * * @see https://github.com/json-editor/json-editor
 * * @license MIT
 * * @example see README.md and docs/ for requirements, examples and usage info
 * * /
 */
!function (t, e) {
    if ("object" == typeof exports && "object" == typeof module) module.exports = e(); else if ("function" == typeof define && define.amd) define([], e); else {
        var s = e();
        for (var i in s) ("object" == typeof exports ? exports : t)[i] = s[i]
    }
}(window, (function () {
    return function (t) {
        var e = {};

        function s(i) {
            if (e[i]) return e[i].exports;
            var r = e[i] = {i: i, l: !1, exports: {}};
            return t[i].call(r.exports, r, r.exports, s), r.l = !0, r.exports
        }

        return s.m = t, s.c = e, s.d = function (t, e, i) {
            s.o(t, e) || Object.defineProperty(t, e, {enumerable: !0, get: i})
        }, s.r = function (t) {
            "undefined" != typeof Symbol && Symbol.toStringTag && Object.defineProperty(t, Symbol.toStringTag, {value: "Module"}), Object.defineProperty(t, "__esModule", {value: !0})
        }, s.t = function (t, e) {
            if (1 & e && (t = s(t)), 8 & e) return t;
            if (4 & e && "object" == typeof t && t && t.__esModule) return t;
            var i = Object.create(null);
            if (s.r(i), Object.defineProperty(i, "default", {
                enumerable: !0,
                value: t
            }), 2 & e && "string" != typeof t) for (var r in t) s.d(i, r, function (e) {
                return t[e]
            }.bind(null, r));
            return i
        }, s.n = function (t) {
            var e = t && t.__esModule ? function () {
                return t.default
            } : function () {
                return t
            };
            return s.d(e, "a", e), e
        }, s.o = function (t, e) {
            return Object.prototype.hasOwnProperty.call(t, e)
        }, s.p = "/dist/", s(s.s = 3)
    }([function (t, e, s) {
        var i = s(1), r = s(2);
        "string" == typeof (r = r.__esModule ? r.default : r) && (r = [[t.i, r, ""]]);
        var o = {insert: "head", singleton: !1};
        i(r, o);
        t.exports = r.locals || {}
    }, function (t, e, s) {
        var i, r = function () {
            return void 0 === i && (i = Boolean(window && document && document.all && !window.atob)), i
        }, o = function () {
            var t = {};
            return function (e) {
                if (void 0 === t[e]) {
                    var s = document.querySelector(e);
                    if (window.HTMLIFrameElement && s instanceof window.HTMLIFrameElement) try {
                        s = s.contentDocument.head
                    } catch (t) {
                        s = null
                    }
                    t[e] = s
                }
                return t[e]
            }
        }(), n = [];

        function a(t) {
            for (var e = -1, s = 0; s < n.length; s++) if (n[s].identifier === t) {
                e = s;
                break
            }
            return e
        }

        function l(t, e) {
            for (var s = {}, i = [], r = 0; r < t.length; r++) {
                var o = t[r], l = e.base ? o[0] + e.base : o[0], h = s[l] || 0, d = "".concat(l, " ").concat(h);
                s[l] = h + 1;
                var c = a(d), p = {css: o[1], media: o[2], sourceMap: o[3]};
                -1 !== c ? (n[c].references++, n[c].updater(p)) : n.push({
                    identifier: d,
                    updater: g(p, e),
                    references: 1
                }), i.push(d)
            }
            return i
        }

        function h(t) {
            var e = document.createElement("style"), i = t.attributes || {};
            if (void 0 === i.nonce) {
                var r = s.nc;
                r && (i.nonce = r)
            }
            if (Object.keys(i).forEach((function (t) {
                e.setAttribute(t, i[t])
            })), "function" == typeof t.insert) t.insert(e); else {
                var n = o(t.insert || "head");
                if (!n) throw new Error("Couldn't find a style target. This probably means that the value for the 'insert' parameter is invalid.");
                n.appendChild(e)
            }
            return e
        }

        var d, c = (d = [], function (t, e) {
            return d[t] = e, d.filter(Boolean).join("\n")
        });

        function p(t, e, s, i) {
            var r = s ? "" : i.media ? "@media ".concat(i.media, " {").concat(i.css, "}") : i.css;
            if (t.styleSheet) t.styleSheet.cssText = c(e, r); else {
                var o = document.createTextNode(r), n = t.childNodes;
                n[e] && t.removeChild(n[e]), n.length ? t.insertBefore(o, n[e]) : t.appendChild(o)
            }
        }

        function u(t, e, s) {
            var i = s.css, r = s.media, o = s.sourceMap;
            if (r ? t.setAttribute("media", r) : t.removeAttribute("media"), o && btoa && (i += "\n/*# sourceMappingURL=data:application/json;base64,".concat(btoa(unescape(encodeURIComponent(JSON.stringify(o)))), " */")), t.styleSheet) t.styleSheet.cssText = i; else {
                for (; t.firstChild;) t.removeChild(t.firstChild);
                t.appendChild(document.createTextNode(i))
            }
        }

        var m = null, b = 0;

        function g(t, e) {
            var s, i, r;
            if (e.singleton) {
                var o = b++;
                s = m || (m = h(e)), i = p.bind(null, s, o, !1), r = p.bind(null, s, o, !0)
            } else s = h(e), i = u.bind(null, s, e), r = function () {
                !function (t) {
                    if (null === t.parentNode) return !1;
                    t.parentNode.removeChild(t)
                }(s)
            };
            return i(t), function (e) {
                if (e) {
                    if (e.css === t.css && e.media === t.media && e.sourceMap === t.sourceMap) return;
                    i(t = e)
                } else r()
            }
        }

        t.exports = function (t, e) {
            (e = e || {}).singleton || "boolean" == typeof e.singleton || (e.singleton = r());
            var s = l(t = t || [], e);
            return function (t) {
                if (t = t || [], "[object Array]" === Object.prototype.toString.call(t)) {
                    for (var i = 0; i < s.length; i++) {
                        var r = a(s[i]);
                        n[r].references--
                    }
                    for (var o = l(t, e), h = 0; h < s.length; h++) {
                        var d = a(s[h]);
                        0 === n[d].references && (n[d].updater(), n.splice(d, 1))
                    }
                    s = o
                }
            }
        }
    }, function (t, e) {
        t.exports = ""
    }, function (t, e, s) {
        s.r(e), s.d(e, "JSONEditor", (function () {
            return K
        }));
        const i = ["actionscript", "batchfile", "c", "c++", "cpp", "coffee", "csharp", "css", "dart", "django", "ejs", "erlang", "golang", "groovy", "handlebars", "haskell", "haxe", "html", "ini", "jade", "java", "javascript", "json", "less", "lisp", "lua", "makefile", "matlab", "mysql", "objectivec", "pascal", "perl", "pgsql", "php", "python", "r", "ruby", "sass", "scala", "scss", "smarty", "sql", "sqlserver", "stylus", "svg", "twig", "vbscript", "xml", "yaml"],
            r = [t => "string" === t.type && "color" === t.format && "colorpicker", t => "string" === t.type && ["ip", "ipv4", "ipv6", "hostname"].includes(t.format) && "ip", t => "string" === t.type && i.includes(t.format) && "ace", t => "string" === t.type && ["xhtml", "bbcode"].includes(t.format) && "sceditor", t => "string" === t.type && "markdown" === t.format && "simplemde", t => "string" === t.type && "jodit" === t.format && "jodit", t => "string" === t.type && "autocomplete" === t.format && "autocomplete", t => "string" === t.type && "uuid" === t.format && "uuid", t => "info" === t.format && "info", t => "button" === t.format && "button", t => {
                if (t.links) for (let e = 0; e < t.links.length; e++) if (t.links[e].rel && "describedby" === t.links[e].rel.toLowerCase()) return "describedBy"
            }, t => ["string", "integer"].includes(t.type) && ["starrating", "rating"].includes(t.format) && "starrating", t => ["string", "integer"].includes(t.type) && ["date", "time", "datetime-local"].includes(t.format) && "datetime", t => (t.oneOf || t.anyOf) && "multiple", t => {
                if ("array" === t.type && t.items && !Array.isArray(t.items) && ["string", "number", "integer"].includes(t.items.type)) {
                    if ("choices" === t.format) return "arrayChoices";
                    if (t.uniqueItems) {
                        if ("selectize" === t.format) return "arraySelectize";
                        if ("select2" === t.format) return "arraySelect2";
                        if ("table" !== t.format) return "multiselect"
                    }
                }
            }, t => {
                if (t.enum) {
                    if ("array" === t.type || "object" === t.type) return "enum";
                    if ("number" === t.type || "integer" === t.type || "string" === t.type) return "radio" === t.format ? "radio" : "select2" === t.format ? "select2" : "selectize" === t.format ? "selectize" : "choices" === t.format ? "choices" : "select"
                }
            }, t => {
                if (t.enumSource) return "radio" === t.format ? "radio" : "select2" === t.format ? "select2" : "selectize" === t.format ? "selectize" : "choices" === t.format ? "choices" : "select"
            }, t => "array" === t.type && "table" === t.format && "table", t => "string" === t.type && "url" === t.format && window.FileReader && t.options && t.options.upload === Object(t.options.upload) && "upload", t => "string" === t.type && t.media && "base64" === t.media.binaryEncoding && "base64", t => "any" === t.type && "multiple", t => {
                if ("boolean" === t.type) return "checkbox" === t.format || t.options && t.options.checkbox ? "checkbox" : "select2" === t.format ? "select2" : "selectize" === t.format ? "selectize" : "choices" === t.format ? "choices" : "select"
            }, t => "string" === t.type && "signature" === t.format && "signature", t => "string" == typeof t.type && t.type, t => !t.type && t.properties && "object", t => "string" != typeof t.type && "multiple"],
            o = {}, n = {};
        n.en = {
            error_notset: "Property must be set",
            error_notempty: "Value required",
            error_enum: "Value must be one of the enumerated values",
            error_anyOf: "Value must validate against at least one of the provided schemas",
            error_oneOf: "Value must validate against exactly one of the provided schemas. It currently validates against {{0}} of the schemas.",
            error_not: "Value must not validate against the provided schema",
            error_type_union: "Value must be one of the provided types",
            error_type: "Value must be of type {{0}}",
            error_disallow_union: "Value must not be one of the provided disallowed types",
            error_disallow: "Value must not be of type {{0}}",
            error_multipleOf: "Value must be a multiple of {{0}}",
            error_maximum_excl: "Value must be less than {{0}}",
            error_maximum_incl: "Value must be at most {{0}}",
            error_minimum_excl: "Value must be greater than {{0}}",
            error_minimum_incl: "Value must be at least {{0}}",
            error_maxLength: "Value must be at most {{0}} characters long",
            error_minLength: "Value must be at least {{0}} characters long",
            error_pattern: "Value must match the pattern {{0}}",
            error_additionalItems: "No additional items allowed in this array",
            error_maxItems: "Value must have at most {{0}} items",
            error_minItems: "Value must have at least {{0}} items",
            error_uniqueItems: "Array must have unique items",
            error_maxProperties: "Object must have at most {{0}} properties",
            error_minProperties: "Object must have at least {{0}} properties",
            error_required: "Object is missing the required property '{{0}}'",
            error_additional_properties: "No additional properties allowed, but property {{0}} is set",
            error_dependency: "Must have property {{0}}",
            error_date: "Date must be in the format {{0}}",
            error_time: "Time must be in the format {{0}}",
            error_datetime_local: "Datetime must be in the format {{0}}",
            error_invalid_epoch: "Date must be greater than 1 January 1970",
            error_ipv4: "Value must be a valid IPv4 address in the form of 4 numbers between 0 and 255, separated by dots",
            error_ipv6: "Value must be a valid IPv6 address",
            error_hostname: "The hostname has the wrong format",
            button_delete_all: "All",
            button_delete_all_title: "Delete All",
            button_delete_last: "Last {{0}}",
            button_delete_last_title: "Delete Last {{0}}",
            button_add_row_title: "Add {{0}}",
            button_move_down_title: "Move down",
            button_move_up_title: "Move up",
            button_object_properties: "Object Properties",
            button_delete_row_title: "Delete {{0}}",
            button_delete_row_title_short: "Delete",
            button_copy_row_title_short: "Copy",
            button_collapse: "Collapse",
            button_expand: "Expand",
            flatpickr_toggle_button: "Toggle",
            flatpickr_clear_button: "Clear",
            choices_placeholder_text: "Start typing to add value",
            default_array_item_title: "item",
            button_delete_node_warning: "Are you sure you want to remove this node?"
        }, Object.entries(o).forEach(([t, e]) => {
            o[t].options = e.options || {}
        });
        const a = {
            options: {
                upload: function (t, e, s) {
                    console.log("Upload handler required for upload editor")
                }, prompt_before_delete: !0, use_default_values: !0, max_depth: 0
            },
            theme: "html",
            template: "default",
            themes: {},
            callbacks: {},
            templates: {},
            iconlibs: {},
            editors: o,
            languages: n,
            resolvers: r,
            custom_validators: [],
            default_language: "en",
            language: "en",
            translate: function (t, e) {
                const s = a.languages[a.language];
                if (!s) throw new Error("Unknown language " + a.language);
                let i = s[t] || a.languages.en[t];
                if (void 0 === i) throw new Error("Unknown translate string " + t);
                if (e) for (let t = 0; t < e.length; t++) i = i.replace(new RegExp(`\\{\\{${t}}}`, "g"), e[t]);
                return i
            }
        };

        function l(t, e, s, i) {
            try {
                switch (t.format) {
                    case"ipv4":
                        (t => {
                            const e = t.split(".");
                            if (4 !== e.length) throw new Error("error_ipv4");
                            e.forEach(t => {
                                if (isNaN(+t) || +t < 0 || +t > 255) throw new Error("error_ipv4")
                            })
                        })(e);
                        break;
                    case"ipv6":
                        (t => {
                            if (!t.match("^(?:(?:(?:[a-fA-F0-9]{1,4}:){6}|(?=(?:[a-fA-F0-9]{0,4}:){2,6}(?:[0-9]{1,3}.){3}[0-9]{1,3}$)(([0-9a-fA-F]{1,4}:){1,5}|:)((:[0-9a-fA-F]{1,4}){1,5}:|:)|::(?:[a-fA-F0-9]{1,4}:){5})(?:(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9]).){3}(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])|(?:[a-fA-F0-9]{1,4}:){7}[a-fA-F0-9]{1,4}|(?=(?:[a-fA-F0-9]{0,4}:){0,7}[a-fA-F0-9]{0,4}$)(([0-9a-fA-F]{1,4}:){1,7}|:)((:[0-9a-fA-F]{1,4}){1,7}|:)|(?:[a-fA-F0-9]{1,4}:){7}:|:(:[a-fA-F0-9]{1,4}){7})$")) throw new Error("error_ipv6")
                        })(e);
                        break;
                    case"hostname":
                        (t => {
                            if (!t.match("(?=^.{4,253}$)(^((?!-)[a-zA-Z0-9-]{0,62}[a-zA-Z0-9].)+[a-zA-Z]{2,63}$)")) throw new Error("error_hostname")
                        })(e)
                }
                return []
            } catch (t) {
                return [{path: s, property: "format", message: i(t.message)}]
            }
        }

        function h(t) {
            return null !== t && ("object" == typeof t && !t.nodeType && t !== t.window && !(t.constructor && !u(t.constructor.prototype, "isPrototypeOf")))
        }

        function d(t) {
            return h(t) ? c({}, t) : Array.isArray(t) ? t.map(d) : t
        }

        function c(t, ...e) {
            return e.forEach(e => {
                Object.keys(e).forEach(s => {
                    e[s] && h(e[s]) ? (u(t, s) || (t[s] = {}), c(t[s], e[s])) : Array.isArray(e[s]) ? t[s] = d(e[s]) : t[s] = e[s]
                })
            }), t
        }

        function p(t, e) {
            const s = document.createEvent("HTMLEvents");
            s.initEvent(e, !0, !0), t.dispatchEvent(s)
        }

        function u(t, e) {
            return t && Object.prototype.hasOwnProperty.call(t, e)
        }

        const m = /^\s*(-|\+)?(\d+|(\d*(\.\d*)))([eE][+-]?\d+)?\s*$/;
        const b = /^\s*(-|\+)?(\d+)\s*$/;

        class g {
            constructor(t, e, s, i) {
                this.jsoneditor = t, this.schema = e || this.jsoneditor.schema, this.options = s || {}, this.translate = this.jsoneditor.translate || i.translate, this.defaults = i, this._validateSubSchema = {
                    enum(t, e, s) {
                        const i = JSON.stringify(e);
                        return t.enum.some(t => i === JSON.stringify(t)) ? [] : [{
                            path: s,
                            property: "enum",
                            message: this.translate("error_enum")
                        }]
                    }, extends(t, e, s) {
                        return t.extends.reduce((t, i) => (t.push(...this._validateSchema(i, e, s)), t), [])
                    }, allOf(t, e, s) {
                        return t.allOf.reduce((t, i) => (t.push(...this._validateSchema(i, e, s)), t), [])
                    }, anyOf(t, e, s) {
                        return t.anyOf.some(t => !this._validateSchema(t, e, s).length) ? [] : [{
                            path: s,
                            property: "anyOf",
                            message: this.translate("error_anyOf")
                        }]
                    }, oneOf(t, e, s) {
                        let i = 0;
                        const r = [];
                        t.oneOf.forEach((t, o) => {
                            const n = this._validateSchema(t, e, s);
                            n.length || i++, n.forEach(t => {
                                t.path = `${s}.oneOf[${o}]${t.path.substr(s.length)}`
                            }), r.push(...n)
                        });
                        const o = [];
                        return 1 !== i && (o.push({
                            path: s,
                            property: "oneOf",
                            message: this.translate("error_oneOf", [i])
                        }), o.push(...r)), o
                    }, not(t, e, s) {
                        return this._validateSchema(t.not, e, s).length ? [] : [{
                            path: s,
                            property: "not",
                            message: this.translate("error_not")
                        }]
                    }, type(t, e, s) {
                        if (Array.isArray(t.type)) {
                            if (!t.type.some(t => this._checkType(t, e))) return [{
                                path: s,
                                property: "type",
                                message: this.translate("error_type_union")
                            }]
                        } else if (["date", "time", "datetime-local"].includes(t.format) && "integer" === t.type) {
                            if (!this._checkType("string", "" + e)) return [{
                                path: s,
                                property: "type",
                                message: this.translate("error_type", [t.format])
                            }]
                        } else if (!this._checkType(t.type, e)) return [{
                            path: s,
                            property: "type",
                            message: this.translate("error_type", [t.type])
                        }];
                        return []
                    }, disallow(t, e, s) {
                        if (Array.isArray(t.disallow)) {
                            if (t.disallow.some(t => this._checkType(t, e))) return [{
                                path: s,
                                property: "disallow",
                                message: this.translate("error_disallow_union")
                            }]
                        } else if (this._checkType(t.disallow, e)) return [{
                            path: s,
                            property: "disallow",
                            message: this.translate("error_disallow", [t.disallow])
                        }];
                        return []
                    }
                }, this._validateNumberSubSchema = {
                    multipleOf(t, e, s) {
                        return this._validateNumberSubSchemaMultipleDivisible(t, e, s)
                    }, divisibleBy(t, e, s) {
                        return this._validateNumberSubSchemaMultipleDivisible(t, e, s)
                    }, maximum(t, e, s) {
                        let i = t.exclusiveMaximum ? e < t.maximum : e <= t.maximum;
                        return window.math ? i = window.math[t.exclusiveMaximum ? "smaller" : "smallerEq"](window.math.bignumber(e), window.math.bignumber(t.maximum)) : window.Decimal && (i = new window.Decimal(e)[t.exclusiveMaximum ? "lt" : "lte"](new window.Decimal(t.maximum))), i ? [] : [{
                            path: s,
                            property: "maximum",
                            message: this.translate(t.exclusiveMaximum ? "error_maximum_excl" : "error_maximum_incl", [t.maximum])
                        }]
                    }, minimum(t, e, s) {
                        let i = t.exclusiveMinimum ? e > t.minimum : e >= t.minimum;
                        return window.math ? i = window.math[t.exclusiveMinimum ? "larger" : "largerEq"](window.math.bignumber(e), window.math.bignumber(t.minimum)) : window.Decimal && (i = new window.Decimal(e)[t.exclusiveMinimum ? "gt" : "gte"](new window.Decimal(t.minimum))), i ? [] : [{
                            path: s,
                            property: "minimum",
                            message: this.translate(t.exclusiveMinimum ? "error_minimum_excl" : "error_minimum_incl", [t.minimum])
                        }]
                    }
                }, this._validateStringSubSchema = {
                    maxLength(t, e, s) {
                        const i = [];
                        return ("" + e).length > t.maxLength && i.push({
                            path: s,
                            property: "maxLength",
                            message: this.translate("error_maxLength", [t.maxLength])
                        }), i
                    }, minLength(t, e, s) {
                        return ("" + e).length < t.minLength ? [{
                            path: s,
                            property: "minLength",
                            message: this.translate(1 === t.minLength ? "error_notempty" : "error_minLength", [t.minLength])
                        }] : []
                    }, pattern(t, e, s) {
                        return new RegExp(t.pattern).test(e) ? [] : [{
                            path: s,
                            property: "pattern",
                            message: t.options && t.options.patternmessage ? t.options.patternmessage : this.translate("error_pattern", [t.pattern])
                        }]
                    }
                }, this._validateArraySubSchema = {
                    items(t, e, s) {
                        const i = [];
                        if (Array.isArray(t.items)) for (let r = 0; r < e.length; r++) if (t.items[r]) console.log("--\x3e"), i.push(...this._validateSchema(t.items[r], e[r], `${s}.${r}`)); else {
                            if (!0 === t.additionalItems) break;
                            if (!t.additionalItems) {
                                if (!1 === t.additionalItems) {
                                    i.push({
                                        path: s,
                                        property: "additionalItems",
                                        message: this.translate("error_additionalItems")
                                    });
                                    break
                                }
                                break
                            }
                            i.push(...this._validateSchema(t.additionalItems, e[r], `${s}.${r}`))
                        } else e.forEach((e, r) => {
                            i.push(...this._validateSchema(t.items, e, `${s}.${r}`))
                        });
                        return i
                    }, maxItems(t, e, s) {
                        return e.length > t.maxItems ? [{
                            path: s,
                            property: "maxItems",
                            message: this.translate("error_maxItems", [t.maxItems])
                        }] : []
                    }, minItems(t, e, s) {
                        return e.length < t.minItems ? [{
                            path: s,
                            property: "minItems",
                            message: this.translate("error_minItems", [t.minItems])
                        }] : []
                    }, uniqueItems(t, e, s) {
                        const i = {};
                        for (let t = 0; t < e.length; t++) {
                            const r = JSON.stringify(e[t]);
                            if (i[r]) return [{
                                path: s,
                                property: "uniqueItems",
                                message: this.translate("error_uniqueItems")
                            }];
                            i[r] = !0
                        }
                        return []
                    }
                }, this._validateObjectSubSchema = {
                    maxProperties(t, e, s) {
                        return Object.keys(e).length > t.maxProperties ? [{
                            path: s,
                            property: "maxProperties",
                            message: this.translate("error_maxProperties", [t.maxProperties])
                        }] : []
                    }, minProperties(t, e, s) {
                        return Object.keys(e).length < t.minProperties ? [{
                            path: s,
                            property: "minProperties",
                            message: this.translate("error_minProperties", [t.minProperties])
                        }] : []
                    }, required(t, e, s) {
                        const i = [];
                        return Array.isArray(t.required) && t.required.forEach(t => {
                            if (void 0 !== e[t]) return;
                            const r = this.jsoneditor.getEditor(`${s}.${t}`);
                            r && ["button", "info"].includes(r.schema.format || r.schema.type) || i.push({
                                path: s,
                                property: "required",
                                message: this.translate("error_required", [t])
                            })
                        }), i
                    }, properties(t, e, s, i) {
                        const r = [];
                        return Object.entries(t.properties).forEach(([t, o]) => {
                            i[t] = !0, r.push(...this._validateSchema(o, e[t], `${s}.${t}`))
                        }), r
                    }, patternProperties(t, e, s, i) {
                        const r = [];
                        return Object.entries(t.patternProperties).forEach(([t, o]) => {
                            const n = new RegExp(t);
                            Object.entries(e).forEach(([t, e]) => {
                                n.test(t) && (i[t] = !0, r.push(...this._validateSchema(o, e, `${s}.${t}`)))
                            })
                        }), r
                    }
                }, this._validateObjectSubSchema2 = {
                    additionalProperties(t, e, s, i) {
                        const r = [], o = Object.keys(e);
                        for (let n = 0; n < o.length; n++) {
                            const a = o[n];
                            if (!i[a]) {
                                if (!t.additionalProperties) {
                                    r.push({
                                        path: s,
                                        property: "additionalProperties",
                                        message: this.translate("error_additional_properties", [a])
                                    });
                                    break
                                }
                                if (!0 === t.additionalProperties) break;
                                r.push(...this._validateSchema(t.additionalProperties, e[a], `${s}.${a}`))
                            }
                        }
                        return r
                    }, dependencies(t, e, s) {
                        const i = [];
                        return Object.entries(t.dependencies).forEach(([t, r]) => {
                            void 0 !== e[t] && (Array.isArray(r) ? r.forEach(t => {
                                void 0 === e[t] && i.push({
                                    path: s,
                                    property: "dependencies",
                                    message: this.translate("error_dependency", [t])
                                })
                            }) : i.push(...this._validateSchema(r, e, s)))
                        }), i
                    }
                }
            }

            fitTest(t, e, s = 1e7) {
                const i = {match: 0, extra: 0};
                if ("object" == typeof t && null !== t) {
                    const r = this._getSchema(e).properties;
                    for (const e in r) if (u(r, e)) {
                        if ("object" == typeof t[e] && "object" == typeof r[e] && "object" == typeof r[e].properties) {
                            const o = this.fitTest(t[e], r[e], s / 100);
                            i.match += o.match, i.extra += o.extra
                        }
                        void 0 !== t[e] && (i.match += s)
                    } else i.extra += s
                }
                return i
            }

            _getSchema(t) {
                return void 0 === t ? c({}, this.jsoneditor.expandRefs(this.schema)) : t
            }

            validate(t) {
                return this._validateSchema(this.schema, t)
            }

            _validateSchema(t, e, s) {
                const i = [];
                return s = s || "root", t = c({}, this.jsoneditor.expandRefs(t)), void 0 === e ? this._validateV3Required(t, e, s) : (Object.keys(t).forEach(r => {
                    this._validateSubSchema[r] && i.push(...this._validateSubSchema[r].call(this, t, e, s))
                }), i.push(...this._validateByValueType(t, e, s)), t.links && t.links.forEach((r, o) => {
                    r.rel && "describedby" === r.rel.toLowerCase() && (t = this._expandSchemaLink(t, o), i.push(...this._validateSchema(t, e, s, this.translate)))
                }), ["date", "time", "datetime-local"].includes(t.format) && i.push(...this._validateDateTimeSubSchema(t, e, s)), i.push(...this._validateCustomValidator(t, e, s)), this._removeDuplicateErrors(i))
            }

            _expandSchemaLink(t, e) {
                const s = t.links[e].href, i = this.jsoneditor.root.getValue(),
                    r = this.jsoneditor.compileTemplate(s, this.jsoneditor.template),
                    o = document.location.origin + document.location.pathname + r(i);
                return t.links = t.links.slice(0, e).concat(t.links.slice(e + 1)), c({}, t, this.jsoneditor.refs[o])
            }

            _validateV3Required(t, e, s) {
                return void 0 !== t.required && !0 === t.required || void 0 === t.required && !0 === this.jsoneditor.options.required_by_default ? [{
                    path: s,
                    property: "required",
                    message: this.translate("error_notset")
                }] : []
            }

            _validateByValueType(t, e, s) {
                const i = [];
                if (null === e) return i;
                if ("number" == typeof e) Object.keys(t).forEach(r => {
                    this._validateNumberSubSchema[r] && i.push(...this._validateNumberSubSchema[r].call(this, t, e, s))
                }); else if ("string" == typeof e) Object.keys(t).forEach(r => {
                    this._validateStringSubSchema[r] && i.push(...this._validateStringSubSchema[r].call(this, t, e, s))
                }); else if (Array.isArray(e)) Object.keys(t).forEach(r => {
                    this._validateArraySubSchema[r] && i.push(...this._validateArraySubSchema[r].call(this, t, e, s))
                }); else if ("object" == typeof e) {
                    const r = {};
                    Object.keys(t).forEach(o => {
                        this._validateObjectSubSchema[o] && i.push(...this._validateObjectSubSchema[o].call(this, t, e, s, r))
                    }), void 0 !== t.additionalProperties || !this.jsoneditor.options.no_additional_properties || t.oneOf || t.anyOf || t.allOf || (t.additionalProperties = !1), Object.keys(t).forEach(o => {
                        void 0 !== this._validateObjectSubSchema2[o] && i.push(...this._validateObjectSubSchema2[o].call(this, t, e, s, r))
                    })
                }
                return i
            }

            _validateNumberSubSchemaMultipleDivisible(t, e, s) {
                const i = t.multipleOf || t.divisibleBy;
                let r = e / i === Math.floor(e / i);
                return window.math ? r = window.math.mod(window.math.bignumber(e), window.math.bignumber(i)).equals(0) : window.Decimal && (r = new window.Decimal(e).mod(new window.Decimal(i)).equals(0)), r ? [] : [{
                    path: s,
                    property: t.multipleOf ? "multipleOf" : "divisibleBy",
                    message: this.translate("error_multipleOf", [i])
                }]
            }

            _validateDateTimeSubSchema(t, e, s) {
                const i = (t, e, s) => 1 * e < 1 ? [{
                    path: s,
                    property: "format",
                    message: this.translate("error_invalid_epoch")
                }] : e !== Math.abs(parseInt(e)) ? [{
                    path: s,
                    property: "format",
                    message: this.translate("error_" + t.format.replace(/-/g, "_"), [a])
                }] : [], r = (t, e, s, i) => {
                    if ("" !== e) {
                        let t;
                        if ("single" !== i.flatpickr.config.mode) {
                            const e = "range" === i.flatpickr.config.mode ? i.flatpickr.l10n.rangeSeparator : ", ";
                            t = i.flatpickr.selectedDates.map(t => i.flatpickr.formatDate(t, i.flatpickr.config.dateFormat)).join(e)
                        }
                        try {
                            if (t) {
                                if (t !== e) throw new Error(i.flatpickr.config.mode + " mismatch")
                            } else if (i.flatpickr.formatDate(i.flatpickr.parseDate(e, i.flatpickr.config.dateFormat), i.flatpickr.config.dateFormat) !== e) throw new Error("mismatch")
                        } catch (t) {
                            const e = void 0 !== i.flatpickr.config.errorDateFormat ? i.flatpickr.config.errorDateFormat : i.flatpickr.config.dateFormat;
                            return [{
                                path: s,
                                property: "format",
                                message: this.translate("error_" + i.format.replace(/-/g, "_"), [e])
                            }]
                        }
                    }
                    return []
                }, o = {
                    date: /^(\d{4}\D\d{2}\D\d{2})?$/,
                    time: /^(\d{2}:\d{2}(?::\d{2})?)?$/,
                    "datetime-local": /^(\d{4}\D\d{2}\D\d{2}[ T]\d{2}:\d{2}(?::\d{2})?)?$/
                }, n = this.jsoneditor.getEditor(s), a = n && n.flatpickr ? n.flatpickr.config.dateFormat : {
                    date: '"YYYY-MM-DD"',
                    time: '"HH:MM"',
                    "datetime-local": '"YYYY-MM-DD HH:MM"'
                }[t.format];
                if ("integer" === t.type) return i(t, e, s);
                if (n && n.flatpickr) {
                    if (n) return r(0, e, s, n)
                } else if (!o[t.format].test(e)) return [{
                    path: s,
                    property: "format",
                    message: this.translate("error_" + t.format.replace(/-/g, "_"), [a])
                }];
                return []
            }

            _validateCustomValidator(t, e, s) {
                const i = [];
                i.push(...l.call(this, t, e, s, this.translate));
                const r = r => {
                    i.push(...r.call(this, t, e, s))
                };
                return this.defaults.custom_validators.forEach(r), this.options.custom_validators && this.options.custom_validators.forEach(r), i
            }

            _removeDuplicateErrors(t) {
                return t.reduce((t, e) => {
                    let s = !0;
                    return t || (t = []), t.forEach(t => {
                        t.message === e.message && t.path === e.path && t.property === e.property && (t.errorcount++, s = !1)
                    }), s && (e.errorcount = 1, t.push(e)), t
                }, [])
            }

            _checkType(t, e) {
                const s = {
                    string: t => "string" == typeof t,
                    number: t => "number" == typeof t,
                    integer: t => "number" == typeof t && t === Math.floor(t),
                    boolean: t => "boolean" == typeof t,
                    array: t => Array.isArray(t),
                    object: t => null !== t && !Array.isArray(t) && "object" == typeof t,
                    null: t => null === t
                };
                return "string" == typeof t ? !s[t] || s[t](e) : !this._validateSchema(t, e).length
            }
        }

        class f {
            constructor(t) {
                this.options = t || {}, this.refs = this.options.refs || {}, this.refs_with_info = {}, this.refs_prefix = "#/counter/", this.refs_counter = 1, this._subSchema1 = {
                    type(t) {
                        "object" == typeof t.type && (t.type = this._expandSubSchema(t.type))
                    }, disallow(t) {
                        "object" == typeof t.disallow && (t.disallow = this._expandSubSchema(t.disallow))
                    }, anyOf(t) {
                        Object.entries(t.anyOf).forEach(([e, s]) => {
                            t.anyOf[e] = this.expandSchema(s)
                        })
                    }, dependencies(t) {
                        Object.entries(t.dependencies).forEach(([e, s]) => {
                            "object" != typeof s || Array.isArray(s) || (t.dependencies[e] = this.expandSchema(s))
                        })
                    }, not(t) {
                        t.not = this.expandSchema(t.not)
                    }
                }, this._subSchema2 = {
                    allOf(t, e) {
                        let s = c({}, e);
                        return Object.entries(t.allOf).forEach(([e, i]) => {
                            t.allOf[e] = this.expandRefs(i, !0), s = this.extendSchemas(s, this.expandSchema(i))
                        }), delete s.allOf, s
                    }, extends(t, e) {
                        let s;
                        return s = Array.isArray(t.extends) ? t.extends.reduce((t, e, s) => this.extendSchemas(t, this.expandSchema(e)), e) : this.extendSchemas(e, this.expandSchema(t.extends)), delete s.extends, s
                    }, oneOf(t, e) {
                        const s = c({}, e);
                        return delete s.oneOf, t.oneOf.reduce((t, e, i) => (t.oneOf[i] = this.extendSchemas(this.expandSchema(e), s), t), e), e
                    }
                }
            }

            load(t, e, s, i) {
                this._loadExternalRefs(t, () => {
                    this._getDefinitions(t, s + "#/definitions/"), e(this.expandRefs(t))
                }, s, this._getFileBase(i))
            }

            expandRefs(t, e) {
                const s = c({}, t);
                if (!s.$ref) return s;
                const i = this.refs_with_info[s.$ref];
                delete s.$ref;
                const r = i.$ref.startsWith("#") ? i.fetchUrl : "", o = this._getRef(r, i);
                if (this.refs[o]) {
                    if (e && u(this.refs[o], "allOf")) {
                        const t = this.refs[o].allOf;
                        Object.keys(t).forEach(e => {
                            t[e] = this.expandRefs(t[e], !0)
                        })
                    }
                } else console.warn(`reference:'${o}' not found!`);
                return this.extendSchemas(s, this.expandSchema(this.refs[o]))
            }

            expandSchema(t, e) {
                Object.entries(this._subSchema1).forEach(([e, s]) => {
                    t[e] && s.call(this, t)
                });
                let s = c({}, t);
                return Object.entries(this._subSchema2).forEach(([e, i]) => {
                    t[e] && (s = i.call(this, t, s))
                }), this.expandRefs(s)
            }

            _getRef(t, e) {
                const s = t + e;
                return this.refs[s] ? s : t + decodeURIComponent(e.$ref)
            }

            _expandSubSchema(t) {
                return Array.isArray(t) ? t.map(t => "object" == typeof value ? this.expandSchema(t) : t) : this.expandSchema(t)
            }

            _getDefinitions(t, e) {
                t.definitions && Object.keys(t.definitions).forEach(s => {
                    this.refs[e + s] = t.definitions[s], t.definitions[s].definitions && this._getDefinitions(t.definitions[s], e + s + "/definitions/")
                })
            }

            _getExternalRefs(t, e) {
                const s = {}, i = t => Object.keys(t).forEach(t => {
                    s[t] = !0
                });
                if (t.$ref && "object" != typeof t.$ref) {
                    const i = this.refs_prefix + this.refs_counter++;
                    "#" === t.$ref.substr(0, 1) || this.refs[t.$ref] || (s[t.$ref] = !0), this.refs_with_info[i] = {
                        fetchUrl: e,
                        $ref: t.$ref
                    }, t.$ref = i
                }
                return Object.values(t).forEach(t => {
                    t && "object" == typeof t && (Array.isArray(t) ? Object.values(t).forEach(t => {
                        t && "object" == typeof t && i(this._getExternalRefs(t, e))
                    }) : i(this._getExternalRefs(t, e)))
                }), s
            }

            _getFileBase(t) {
                const {ajaxBase: e} = this.options;
                return void 0 === e ? this._getFileBaseFromFileLocation(t) : e
            }

            _getFileBaseFromFileLocation(t) {
                const e = t.split("/");
                return e.pop(), e.join("/") + "/"
            }

            _isLocalUrl(t, e) {
                return e !== t.substr(0, e.length) && "http" !== t.substr(0, 4) && "/" !== t.substr(0, 1)
            }

            _loadExternalRefs(t, e, s, i) {
                const r = this._getExternalRefs(t, s);
                let o = 0, n = 0, a = !1;
                Object.keys(r).forEach(t => {
                    if (this.refs[t]) return;
                    if (!this.options.ajax) throw new Error("Must set ajax option to true to load external ref " + t);
                    this.refs[t] = "loading", n++;
                    const s = this._isLocalUrl(t, i) ? i + t : t, r = new XMLHttpRequest;
                    r.overrideMimeType("application/json"), r.open("GET", s, !0), this.options.ajaxCredentials && (r.withCredentials = this.options.ajaxCredentials), r.onreadystatechange = () => {
                        if (4 === r.readyState) {
                            if (200 !== r.status) throw window.console.log(r), new Error("Failed to fetch ref via ajax- " + t);
                            {
                                let i;
                                try {
                                    i = JSON.parse(r.responseText)
                                } catch (t) {
                                    throw window.console.log(t), new Error("Failed to parse external ref " + s)
                                }
                                if ("boolean" != typeof i && "object" != typeof i || null === i || Array.isArray(i)) throw new Error("External ref does not contain a valid schema - " + s);
                                this.refs[t] = i;
                                const l = this._getFileBaseFromFileLocation(s);
                                this._getDefinitions(i, s + "#/definitions/"), this._loadExternalRefs(i, () => {
                                    o++, o >= n && !a && (a = !0, e())
                                }, s, l)
                            }
                        }
                    }, r.send()
                }), n || e()
            }

            extendSchemas(t, e) {
                t = c({}, t), e = c({}, e);
                const s = {}, i = (t, i) => {
                    ((t, e) => ("required" === t || "defaultProperties" === t) && "object" == typeof e && Array.isArray(e))(t, i) ? s[t] = i.concat(e[t]).reduce((t, e) => (t.includes(e) || t.push(e), t), []) : "type" !== t || "string" != typeof i && !Array.isArray(i) ? "object" != typeof i || Array.isArray(i) || null === i ? s[t] = i : s[t] = this.extendSchemas(i, e[t]) : r(i)
                }, r = t => {
                    "string" == typeof t && (t = [t]), "string" == typeof e.type && (e.type = [e.type]), e.type && e.type.length ? s.type = t.filter(t => e.type.includes(t)) : s.type = t, 1 === s.type.length && "string" == typeof s.type[0] ? s.type = s.type[0] : 0 === s.type.length && delete s.type
                };
                return Object.entries(t).forEach(([t, r]) => {
                    void 0 !== e[t] ? i(t, r) : s[t] = r
                }), Object.entries(e).forEach(([e, i]) => {
                    void 0 === t[e] && (s[e] = i)
                }), s
            }
        }

        class _ {
            constructor(t, e) {
                this.defaults = e, this.jsoneditor = t.jsoneditor, this.theme = this.jsoneditor.theme, this.template_engine = this.jsoneditor.template, this.iconlib = this.jsoneditor.iconlib, this.translate = this.jsoneditor.translate || this.defaults.translate, this.original_schema = t.schema, this.schema = this.jsoneditor.expandSchema(this.original_schema), this.active = !0, this.options = c({}, this.options || {}, this.schema.options || {}, t.schema.options || {}, t), t.path || this.schema.id || (this.schema.id = "root"), this.path = t.path || "root", this.formname = t.formname || this.path.replace(/\.([^.]+)/g, "[$1]"), this.jsoneditor.options.form_name_root && (this.formname = this.formname.replace(/^root\[/, this.jsoneditor.options.form_name_root + "[")), this.parent = t.parent, this.key = void 0 !== this.parent ? this.path.split(".").slice(this.parent.path.split(".").length).join(".") : this.path, this.link_watchers = [], this.watchLoop = !1, t.container && this.setContainer(t.container), this.registerDependencies()
            }

            onChildEditorChange(t) {
                this.onChange(!0)
            }

            notify() {
                this.path && this.jsoneditor.notifyWatchers(this.path)
            }

            change() {
                this.parent ? this.parent.onChildEditorChange(this) : this.jsoneditor && this.jsoneditor.onChange()
            }

            onChange(t) {
                this.notify(), this.watch_listener && this.watch_listener(), t && this.change()
            }

            register() {
                this.jsoneditor.registerEditor(this), this.onChange()
            }

            unregister() {
                this.jsoneditor && this.jsoneditor.unregisterEditor(this)
            }

            getNumColumns() {
                return 12
            }

            isActive() {
                return this.active
            }

            activate() {
                this.active = !0, this.optInCheckbox.checked = !0, this.enable(), this.change()
            }

            deactivate() {
                this.isRequired() || (this.active = !1, this.optInCheckbox.checked = !1, this.disable(), this.change())
            }

            registerDependencies() {
                this.dependenciesFulfilled = !0;
                const t = this.options.dependencies;
                t && Object.keys(t).forEach(e => {
                    let s = this.path.split(".");
                    s[s.length - 1] = e, s = s.join(".");
                    const i = t[e];
                    this.jsoneditor.watch(s, () => {
                        this.checkDependency(s, i)
                    })
                })
            }

            checkDependency(t, e) {
                const s = this.container || this.control;
                if (this.path === t || !s || null === this.jsoneditor) return;
                const i = this.jsoneditor.getEditor(t), r = i ? i.getValue() : void 0, o = this.dependenciesFulfilled;
                this.dependenciesFulfilled = !1, i && i.dependenciesFulfilled ? Array.isArray(e) ? e.some(t => {
                    if (r === t) return this.dependenciesFulfilled = !0, !0
                }) : "object" == typeof e ? "object" != typeof r ? this.dependenciesFulfilled = e === r : Object.keys(e).some(t => !!u(e, t) && (u(r, t) && e[t] === r[t] ? void (this.dependenciesFulfilled = !0) : (this.dependenciesFulfilled = !1, !0))) : "string" == typeof e || "number" == typeof e ? this.dependenciesFulfilled = r === e : "boolean" == typeof e && (this.dependenciesFulfilled = e ? r || r.length > 0 : !r || 0 === r.length) : this.dependenciesFulfilled = !1, this.dependenciesFulfilled !== o && this.notify();
                const n = this.dependenciesFulfilled ? "block" : "none";
                "TD" === s.tagName ? Object.keys(s.childNodes).forEach(t => s.childNodes[t].style.display = n) : s.style.display = n
            }

            setContainer(t) {
                this.container = t, this.schema.id && this.container.setAttribute("data-schemaid", this.schema.id), this.schema.type && "string" == typeof this.schema.type && this.container.setAttribute("data-schematype", this.schema.type), this.container.setAttribute("data-schemapath", this.path)
            }

            setOptInCheckbox(t) {
                this.optInCheckbox = document.createElement("input"), this.optInCheckbox.setAttribute("type", "checkbox"), this.optInCheckbox.setAttribute("style", "margin: 0 10px 0 0;"), this.optInCheckbox.classList.add("json-editor-opt-in"), this.optInCheckbox.addEventListener("click", () => {
                    this.isActive() ? this.deactivate() : this.activate()
                }), (this.jsoneditor.options.show_opt_in || this.options.show_opt_in) && this.parent && "object" === this.parent.schema.type && !this.isRequired() && this.header && (this.header.appendChild(this.optInCheckbox), this.header.insertBefore(this.optInCheckbox, this.header.firstChild))
            }

            preBuild() {
            }

            build() {
            }

            postBuild() {
                this.setupWatchListeners(), this.addLinks(), this.setValue(this.getDefault(), !0), this.updateHeaderText(), this.register(), this.onWatchedFieldChange()
            }

            setupWatchListeners() {
                if (this.watched = {}, this.schema.vars && (this.schema.watch = this.schema.vars), this.watched_values = {}, this.watch_listener = () => {
                    this.refreshWatchedFieldValues() && this.onWatchedFieldChange()
                }, u(this.schema, "watch")) {
                    let t, e, s, i, r;
                    const o = this.container.getAttribute("data-schemapath");
                    Object.keys(this.schema.watch).forEach(n => {
                        if (t = this.schema.watch[n], Array.isArray(t)) {
                            if (t.length < 2) return;
                            e = [t[0]].concat(t[1].split("."))
                        } else e = t.split("."), this.theme.closest(this.container, `[data-schemaid="${e[0]}"]`) || e.unshift("#");
                        if (s = e.shift(), "#" === s && (s = this.jsoneditor.schema.id || "root"), i = this.theme.closest(this.container, `[data-schemaid="${s}"]`), !i) throw new Error("Could not find ancestor node with id " + s);
                        r = `${i.getAttribute("data-schemapath")}.${e.join(".")}`, o.startsWith(r) && (this.watchLoop = !0), this.jsoneditor.watch(r, this.watch_listener), this.watched[n] = r
                    })
                }
                this.schema.headerTemplate && (this.header_template = this.jsoneditor.compileTemplate(this.schema.headerTemplate, this.template_engine))
            }

            addLinks() {
                if (!this.no_link_holder && (this.link_holder = this.theme.getLinksHolder(), void 0 !== this.description ? this.description.parentNode.insertBefore(this.link_holder, this.description) : this.container.appendChild(this.link_holder), this.schema.links)) for (let t = 0; t < this.schema.links.length; t++) this.addLink(this.getLink(this.schema.links[t]))
            }

            onMove() {
            }

            getButton(t, e, s) {
                const i = "json-editor-btn-" + e;
                !(e = this.iconlib ? this.iconlib.getIcon(e) : null) && s && (t = s, s = null);
                const r = this.theme.getButton(t, e, s);
                return r.classList.add(i), r
            }

            setButtonText(t, e, s, i) {
                return !(s = this.iconlib ? this.iconlib.getIcon(s) : null) && i && (e = i, i = null), this.theme.setButtonText(t, e, s, i)
            }

            addLink(t) {
                this.link_holder && this.link_holder.appendChild(t)
            }

            getLink(t) {
                let e, s;
                const i = (t.mediaType || "application/javascript").split("/")[0],
                    r = this.jsoneditor.compileTemplate(t.href, this.template_engine),
                    o = this.jsoneditor.compileTemplate(t.rel ? t.rel : t.href, this.template_engine);
                let n = null;
                if (t.download && (n = t.download), n && !0 !== n && (n = this.jsoneditor.compileTemplate(n, this.template_engine)), "image" === i) {
                    e = this.theme.getBlockLinkHolder(), s = document.createElement("a"), s.setAttribute("target", "_blank");
                    const t = document.createElement("img");
                    this.theme.createImageLink(e, s, t), this.link_watchers.push(e => {
                        const i = r(e), n = o(e);
                        s.setAttribute("href", i), s.setAttribute("title", n || i), t.setAttribute("src", i)
                    })
                } else if (["audio", "video"].includes(i)) {
                    e = this.theme.getBlockLinkHolder(), s = this.theme.getBlockLink(), s.setAttribute("target", "_blank");
                    const t = document.createElement(i);
                    t.setAttribute("controls", "controls"), this.theme.createMediaLink(e, s, t), this.link_watchers.push(e => {
                        const i = r(e), n = o(e);
                        s.setAttribute("href", i), s.textContent = n || i, t.setAttribute("src", i)
                    })
                } else s = e = this.theme.getBlockLink(), e.setAttribute("target", "_blank"), e.textContent = t.rel, e.style.display = "none", this.link_watchers.push(t => {
                    const s = r(t), i = o(t);
                    s && (e.style.display = ""), e.setAttribute("href", s), e.textContent = i || s
                });
                return n && s && (!0 === n ? s.setAttribute("download", "") : this.link_watchers.push(t => {
                    s.setAttribute("download", n(t))
                })), t.class && s.classList.add(t.class), e
            }

            refreshWatchedFieldValues() {
                if (!this.watched_values) return;
                const t = {};
                let e = !1;
                return this.watched && Object.keys(this.watched).forEach(s => {
                    const i = this.jsoneditor.getEditor(this.watched[s]), r = i ? i.getValue() : null;
                    this.watched_values[s] !== r && (e = !0), t[s] = r
                }), t.self = this.getValue(), this.watched_values.self !== t.self && (e = !0), this.watched_values = t, e
            }

            getWatchedFieldValues() {
                return this.watched_values
            }

            updateHeaderText() {
                if (this.header) {
                    const t = this.getHeaderText();
                    if (this.header.children.length) {
                        for (let e = 0; e < this.header.childNodes.length; e++) if (3 === this.header.childNodes[e].nodeType) {
                            this.header.childNodes[e].nodeValue = this.cleanText(t);
                            break
                        }
                    } else window.DOMPurify ? this.header.innerHTML = window.DOMPurify.sanitize(t) : this.header.textContent = this.cleanText(t)
                }
            }

            getHeaderText(t) {
                return this.header_text ? this.header_text : t ? this.schema.title : this.getTitle()
            }

            cleanText(t) {
                const e = document.createElement("div");
                return e.innerHTML = t, e.textContent || e.innerText
            }

            onWatchedFieldChange() {
                let t;
                if (this.header_template) {
                    t = c(this.getWatchedFieldValues(), {
                        key: this.key,
                        i: this.key,
                        i0: 1 * this.key,
                        i1: 1 * this.key + 1,
                        title: this.getTitle()
                    });
                    const e = this.header_template(t);
                    e !== this.header_text && (this.header_text = e, this.updateHeaderText(), this.notify())
                }
                if (this.link_watchers.length) {
                    t = this.getWatchedFieldValues();
                    for (let e = 0; e < this.link_watchers.length; e++) this.link_watchers[e](t)
                }
            }

            setValue(t) {
                this.value = t
            }

            getValue() {
                if (this.dependenciesFulfilled) return this.value
            }

            refreshValue() {
            }

            getChildEditors() {
                return !1
            }

            destroy() {
                this.unregister(this), this.watched && Object.values(this.watched).forEach(t => this.jsoneditor.unwatch(t, this.watch_listener)), this.watched = null, this.watched_values = null, this.watch_listener = null, this.header_text = null, this.header_template = null, this.value = null, this.container && this.container.parentNode && this.container.parentNode.removeChild(this.container), this.container = null, this.jsoneditor = null, this.schema = null, this.path = null, this.key = null, this.parent = null
            }

            isDefaultRequired() {
                return this.isRequired() || !!this.jsoneditor.options.use_default_values
            }

            getDefault() {
                if (void 0 !== this.schema.default) return this.schema.default;
                if (void 0 !== this.schema.enum) return this.schema.enum[0];
                let t = this.schema.type || this.schema.oneOf;
                if (t && Array.isArray(t) && (t = t[0]), t && "object" == typeof t && (t = t.type), t && Array.isArray(t) && (t = t[0]), "string" == typeof t) {
                    if ("number" === t) return this.isDefaultRequired() ? 0 : void 0;
                    if ("boolean" === t) return !this.isDefaultRequired() && void 0;
                    if ("integer" === t) return this.isDefaultRequired() ? 0 : void 0;
                    if ("string" === t) return "";
                    if ("object" === t) return {};
                    if ("array" === t) return []
                }
                return null
            }

            getTitle() {
                return this.schema.title || this.key
            }

            enable() {
                this.disabled = !1
            }

            disable() {
                this.disabled = !0
            }

            isEnabled() {
                return !this.disabled
            }

            isRequired() {
                return "boolean" == typeof this.schema.required ? this.schema.required : this.parent && this.parent.schema && Array.isArray(this.parent.schema.required) ? this.parent.schema.required.includes(this.key) : !!this.jsoneditor.options.required_by_default
            }

            getDisplayText(t) {
                const e = [], s = {};
                t.forEach(t => {
                    t.title && (s[t.title] = s[t.title] || 0, s[t.title]++), t.description && (s[t.description] = s[t.description] || 0, s[t.description]++), t.format && (s[t.format] = s[t.format] || 0, s[t.format]++), t.type && (s[t.type] = s[t.type] || 0, s[t.type]++)
                }), t.forEach(t => {
                    let i;
                    i = "string" == typeof t ? t : t.title && s[t.title] <= 1 ? t.title : t.format && s[t.format] <= 1 ? t.format : t.type && s[t.type] <= 1 ? t.type : t.description && s[t.description] <= 1 ? t.descripton : t.title ? t.title : t.format ? t.format : t.type ? t.type : t.description ? t.description : JSON.stringify(t).length < 500 ? JSON.stringify(t) : "type", e.push(i)
                });
                const i = {};
                return e.forEach((t, r) => {
                    i[t] = i[t] || 0, i[t]++, s[t] > 1 && (e[r] = `${t} ${i[t]}`)
                }), e
            }

            getValidId(t) {
                return (t = void 0 === t ? "" : t.toString()).replace(/\s+/g, "-")
            }

            setInputAttributes(t) {
                if (this.schema.options && this.schema.options.inputAttributes) {
                    const e = this.schema.options.inputAttributes, s = ["name", "type"].concat(t);
                    Object.keys(e).forEach(t => {
                        s.includes(t.toLowerCase()) || this.input.setAttribute(t, e[t])
                    })
                }
            }

            expandCallbacks(t, e) {
                const s = this.defaults.callbacks[t];
                return Object.entries(e).forEach(([i, r]) => {
                    r === Object(r) ? e[i] = this.expandCallbacks(t, r) : "string" == typeof r && "object" == typeof s && "function" == typeof s[r] && (e[i] = s[r].bind(null, this))
                }), e
            }

            showValidationErrors(t) {
            }
        }

        class y extends _ {
            register() {
                super.register(), this.input && this.input.setAttribute("name", this.formname)
            }

            unregister() {
                super.unregister(), this.input && this.input.removeAttribute("name")
            }

            setValue(t, e, s) {
                if (this.template && !s) return;
                if (null == t ? t = "" : "object" == typeof t ? t = JSON.stringify(t) : "string" != typeof t && (t = "" + t), t === this.serialized) return;
                const i = this.sanitize(t);
                if (this.input.value === i) return;
                if (this.input.value = i, "range" === this.format) {
                    const t = this.control.querySelector("output");
                    t && (t.value = i)
                }
                const r = s || this.getValue() !== t;
                return this.refreshValue(), e ? this.is_dirty = !1 : "change" === this.jsoneditor.options.show_errors && (this.is_dirty = !0), this.adjust_height && this.adjust_height(this.input), this.onChange(r), {
                    changed: r,
                    value: i
                }
            }

            getNumColumns() {
                const t = Math.ceil(Math.max(this.getTitle().length, this.schema.maxLength || 0, this.schema.minLength || 0) / 5);
                let e;
                return e = "textarea" === this.input_type ? 6 : ["text", "email"].includes(this.input_type) ? 4 : 2, Math.min(12, Math.max(t, e))
            }

            build() {
                if (this.options.compact || (this.header = this.label = this.theme.getFormInputLabel(this.getTitle(), this.isRequired())), this.schema.description && (this.description = this.theme.getFormInputDescription(this.schema.description)), this.options.infoText && (this.infoButton = this.theme.getInfoButton(this.options.infoText)), this.format = this.schema.format, !this.format && this.schema.media && this.schema.media.type && (this.format = this.schema.media.type.replace(/(^(application|text)\/(x-)?(script\.)?)|(-source$)/g, "")), !this.format && this.options.default_format && (this.format = this.options.default_format), this.options.format && (this.format = this.options.format), this.format) if ("textarea" === this.format) this.input_type = "textarea", this.input = this.theme.getTextareaInput(); else if ("range" === this.format) {
                    this.input_type = "range";
                    let t = this.schema.minimum || 0, e = this.schema.maximum || Math.max(100, t + 1), s = 1;
                    this.schema.multipleOf && (t % this.schema.multipleOf && (t = Math.ceil(t / this.schema.multipleOf) * this.schema.multipleOf), e % this.schema.multipleOf && (e = Math.floor(e / this.schema.multipleOf) * this.schema.multipleOf), s = this.schema.multipleOf), this.input = this.theme.getRangeInput(t, e, s)
                } else this.input_type = "text", ["button", "checkbox", "color", "date", "datetime-local", "email", "file", "hidden", "image", "month", "number", "password", "radio", "reset", "search", "submit", "tel", "text", "time", "url", "week"].includes(this.format) && (this.input_type = this.format), this.input = this.theme.getFormInputField(this.input_type); else this.input_type = "text", this.input = this.theme.getFormInputField(this.input_type);
                void 0 !== this.schema.maxLength && this.input.setAttribute("maxlength", this.schema.maxLength), void 0 !== this.schema.pattern ? this.input.setAttribute("pattern", this.schema.pattern) : void 0 !== this.schema.minLength && this.input.setAttribute("pattern", `.{${this.schema.minLength},}`), this.options.compact ? this.container.classList.add("compact") : this.options.input_width && (this.input.style.width = this.options.input_width), (this.schema.readOnly || this.schema.readonly || this.schema.template) && (this.always_disabled = !0, this.input.setAttribute("readonly", "true")), this.setInputAttributes(["maxlength", "pattern", "readonly", "min", "max", "step"]), this.input.addEventListener("change", t => {
                    if (t.preventDefault(), t.stopPropagation(), this.schema.template) return void (t.currentTarget.value = this.value);
                    const e = t.currentTarget.value, s = this.sanitize(e);
                    e !== s && (t.currentTarget.value = s), this.is_dirty = !0, this.refreshValue(), this.onChange(!0)
                }), this.options.input_height && (this.input.style.height = this.options.input_height), this.options.expand_height && (this.adjust_height = t => {
                    if (!t) return;
                    let e, s = t.offsetHeight;
                    if (t.offsetHeight < t.scrollHeight) for (e = 0; t.offsetHeight < t.scrollHeight + 3 && !(e > 100);) e++, s++, t.style.height = s + "px"; else {
                        for (e = 0; t.offsetHeight >= t.scrollHeight + 3 && !(e > 100);) e++, s--, t.style.height = s + "px";
                        t.style.height = s + 1 + "px"
                    }
                }, this.input.addEventListener("keyup", t => {
                    this.adjust_height(t.currentTarget)
                }), this.input.addEventListener("change", t => {
                    this.adjust_height(t.currentTarget)
                }), this.adjust_height()), this.format && this.input.setAttribute("data-schemaformat", this.format);
                let {input: t} = this;
                if ("range" === this.format && (t = this.theme.getRangeControl(this.input, this.theme.getRangeOutput(this.input, this.schema.default || Math.max(this.schema.minimum || 0, 0)))), this.control = this.theme.getFormControl(this.label, t, this.description, this.infoButton), this.container.appendChild(this.control), window.requestAnimationFrame(() => {
                    this.input.parentNode && this.afterInputReady(), this.adjust_height && this.adjust_height(this.input)
                }), this.schema.template) {
                    const t = this.expandCallbacks("template", {template: this.schema.template});
                    "function" == typeof t.template ? this.template = t.template : this.template = this.jsoneditor.compileTemplate(this.schema.template, this.template_engine), this.refreshValue()
                } else this.refreshValue()
            }

            setupCleave(t) {
                const e = this.expandCallbacks("cleave", c({}, this.defaults.options.cleave || {}, this.options.cleave || {}));
                "object" == typeof e && Object.keys(e).length > 0 && (this.cleave_instance = new window.Cleave(t, e))
            }

            setupImask(t) {
                const e = this.expandCallbacks("imask", c({}, this.defaults.options.imask || {}, this.options.imask || {}));
                "object" == typeof e && Object.keys(e).length > 0 && (this.imask_instance = window.IMask(t, this.ajustIMaskOptions(e)))
            }

            ajustIMaskOptions(t) {
                return Object.keys(t).forEach(e => {
                    if (t[e] === Object(t[e])) t[e] = this.ajustIMaskOptions(t[e]); else if ("mask" === e) if ("regex:" === t[e].substr(0, 6)) {
                        const s = t[e].match(/^regex:\/(.*)\/([gimsuy]*)$/);
                        if (null !== s) try {
                            t[e] = new RegExp(s[1], s[2])
                        } catch (t) {
                        }
                    } else t[e] = this.getGlobalPropertyFromString(t[e])
                }), t
            }

            getGlobalPropertyFromString(t) {
                if (t.includes(".")) {
                    const e = t.split("."), s = e[0], i = e[1];
                    if (void 0 !== window[s] && void 0 !== window[s][i]) return window[s][i]
                } else if (void 0 !== window[t]) return window[t];
                return t
            }

            getValue() {
                return this.imask_instance && this.dependenciesFulfilled && this.options.imask.returnUnmasked ? this.imask_instance.unmaskedValue : super.getValue()
            }

            enable() {
                this.always_disabled || (this.input.disabled = !1, super.enable())
            }

            disable(t) {
                t && (this.always_disabled = !0), this.input.disabled = !0, super.disable()
            }

            afterInputReady() {
                this.theme.afterInputReady(this.input), window.Cleave && !this.cleave_instance ? this.setupCleave(this.input) : window.IMask && !this.imask_instance && this.setupImask(this.input)
            }

            refreshValue() {
                this.value = this.input.value, "string" != typeof this.value && (this.value = ""), this.serialized = this.value
            }

            destroy() {
                this.cleave_instance && this.cleave_instance.destroy(), this.imask_instance && this.imask_instance.destroy(), this.template = null, this.input && this.input.parentNode && this.input.parentNode.removeChild(this.input), this.label && this.label.parentNode && this.label.parentNode.removeChild(this.label), this.description && this.description.parentNode && this.description.parentNode.removeChild(this.description), super.destroy()
            }

            sanitize(t) {
                return t
            }

            onWatchedFieldChange() {
                let t;
                this.template && (t = this.getWatchedFieldValues(), this.setValue(this.template(t), !1, !0)), super.onWatchedFieldChange()
            }

            showValidationErrors(t) {
                if ("always" === this.jsoneditor.options.show_errors) ; else if (!this.is_dirty && this.previous_error_setting === this.jsoneditor.options.show_errors) return;
                this.previous_error_setting = this.jsoneditor.options.show_errors;
                const e = t.reduce((t, e) => (e.path === this.path && t.push(e.message), t), []);
                e.length ? this.theme.addInputError(this.input, e.join(". ") + ".") : this.theme.removeInputError(this.input)
            }
        }

        class w extends _ {
            askConfirmation() {
                return !0 !== this.jsoneditor.options.prompt_before_delete || !1 !== window.confirm(this.translate("button_delete_node_warning"))
            }

            getDefault() {
                return this.schema.default || []
            }

            register() {
                if (super.register(), this.rows) for (let t = 0; t < this.rows.length; t++) this.rows[t].register()
            }

            unregister() {
                if (super.unregister(), this.rows) for (let t = 0; t < this.rows.length; t++) this.rows[t].unregister()
            }

            getNumColumns() {
                const t = this.getItemInfo(0);
                return this.tabs_holder && "tabs-top" !== this.schema.format ? Math.max(Math.min(12, t.width + 2), 4) : t.width
            }

            enable() {
                if (!this.always_disabled) {
                    if (this.add_row_button && (this.add_row_button.disabled = !1), this.remove_all_rows_button && (this.remove_all_rows_button.disabled = !1), this.delete_last_row_button && (this.delete_last_row_button.disabled = !1), this.copy_button && (this.copy_button.disabled = !1), this.delete_button && (this.delete_button.disabled = !1), this.moveup_button && (this.moveup_button.disabled = !1), this.movedown_button && (this.movedown_button.disabled = !1), this.rows) for (let t = 0; t < this.rows.length; t++) this.rows[t].enable(), this.rows[t].add_row_button && (this.rows[t].add_row_button.disabled = !1), this.rows[t].remove_all_rows_button && (this.rows[t].remove_all_rows_button.disabled = !1), this.rows[t].delete_last_row_button && (this.rows[t].delete_last_row_button.disabled = !1), this.rows[t].copy_button && (this.rows[t].copy_button.disabled = !1), this.rows[t].delete_button && (this.rows[t].delete_button.disabled = !1), this.rows[t].moveup_button && (this.rows[t].moveup_button.disabled = !1), this.rows[t].movedown_button && (this.rows[t].movedown_button.disabled = !1);
                    super.enable()
                }
            }

            disable(t) {
                if (t && (this.always_disabled = !0), this.add_row_button && (this.add_row_button.disabled = !0), this.remove_all_rows_button && (this.remove_all_rows_button.disabled = !0), this.delete_last_row_button && (this.delete_last_row_button.disabled = !0), this.copy_button && (this.copy_button.disabled = !0), this.delete_button && (this.delete_button.disabled = !0), this.moveup_button && (this.moveup_button.disabled = !0), this.movedown_button && (this.movedown_button.disabled = !0), this.rows) for (let e = 0; e < this.rows.length; e++) this.rows[e].disable(t), this.rows[e].add_row_button && (this.rows[e].add_row_button.disabled = !0), this.rows[e].remove_all_rows_button && (this.rows[e].remove_all_rows_button.disabled = !0), this.rows[e].delete_last_row_button && (this.rows[e].delete_last_row_button.disabled = !0), this.rows[e].copy_button && (this.rows[e].copy_button.disabled = !0), this.rows[e].delete_button && (this.rows[e].delete_button.disabled = !0), this.rows[e].moveup_button && (this.rows[e].moveup_button.disabled = !0), this.rows[e].movedown_button && (this.rows[e].movedown_button.disabled = !0);
                super.disable()
            }

            preBuild() {
                super.preBuild(), this.rows = [], this.row_cache = [], this.hide_delete_buttons = this.options.disable_array_delete || this.jsoneditor.options.disable_array_delete, this.hide_delete_all_rows_buttons = this.hide_delete_buttons || this.options.disable_array_delete_all_rows || this.jsoneditor.options.disable_array_delete_all_rows, this.hide_delete_last_row_buttons = this.hide_delete_buttons || this.options.disable_array_delete_last_row || this.jsoneditor.options.disable_array_delete_last_row, this.hide_move_buttons = this.options.disable_array_reorder || this.jsoneditor.options.disable_array_reorder, this.hide_add_button = this.options.disable_array_add || this.jsoneditor.options.disable_array_add, this.show_copy_button = this.options.enable_array_copy || this.jsoneditor.options.enable_array_copy, this.array_controls_top = this.options.array_controls_top || this.jsoneditor.options.array_controls_top
            }

            build() {
                this.options.compact ? (this.title = this.theme.getHeader(""), this.container.appendChild(this.title), this.panel = this.theme.getIndentedPanel(), this.container.appendChild(this.panel), this.title_controls = this.theme.getHeaderButtonHolder(), this.title.appendChild(this.title_controls), this.controls = this.theme.getHeaderButtonHolder(), this.title.appendChild(this.controls), this.row_holder = document.createElement("div"), this.panel.appendChild(this.row_holder)) : (this.header = document.createElement("label"), this.header.textContent = this.getTitle(), this.title = this.theme.getHeader(this.header), this.container.appendChild(this.title), this.title_controls = this.theme.getHeaderButtonHolder(), this.title.appendChild(this.title_controls), this.schema.description && (this.description = this.theme.getDescription(this.schema.description), this.container.appendChild(this.description)), this.error_holder = document.createElement("div"), this.container.appendChild(this.error_holder), "tabs-top" === this.schema.format ? (this.controls = this.theme.getHeaderButtonHolder(), this.title.appendChild(this.controls), this.tabs_holder = this.theme.getTopTabHolder(this.getValidId(this.getItemTitle())), this.container.appendChild(this.tabs_holder), this.row_holder = this.theme.getTopTabContentHolder(this.tabs_holder), this.active_tab = null) : "tabs" === this.schema.format ? (this.controls = this.theme.getHeaderButtonHolder(), this.title.appendChild(this.controls), this.tabs_holder = this.theme.getTabHolder(this.getValidId(this.getItemTitle())), this.container.appendChild(this.tabs_holder), this.row_holder = this.theme.getTabContentHolder(this.tabs_holder), this.active_tab = null) : (this.panel = this.theme.getIndentedPanel(), this.container.appendChild(this.panel), this.row_holder = document.createElement("div"), this.panel.appendChild(this.row_holder), this.controls = this.theme.getButtonHolder(), this.array_controls_top ? this.title.appendChild(this.controls) : this.panel.appendChild(this.controls))), this.addControls()
            }

            onChildEditorChange(t) {
                this.refreshValue(), this.refreshTabs(!0), super.onChildEditorChange(t)
            }

            getItemTitle() {
                if (!this.item_title) if (this.schema.items && !Array.isArray(this.schema.items)) {
                    const t = this.jsoneditor.expandRefs(this.schema.items);
                    this.item_title = t.title || this.translate("default_array_item_title")
                } else this.item_title = this.translate("default_array_item_title");
                return this.cleanText(this.item_title)
            }

            getItemSchema(t) {
                return Array.isArray(this.schema.items) ? t >= this.schema.items.length ? !0 === this.schema.additionalItems ? {} : this.schema.additionalItems ? c({}, this.schema.additionalItems) : void 0 : c({}, this.schema.items[t]) : this.schema.items ? c({}, this.schema.items) : {}
            }

            getItemInfo(t) {
                let e = this.getItemSchema(t);
                this.item_info = this.item_info || {};
                const s = JSON.stringify(e);
                return void 0 !== this.item_info[s] || (e = this.jsoneditor.expandRefs(e), this.item_info[s] = {
                    title: e.title || this.translate("default_array_item_title"),
                    default: e.default,
                    width: 12,
                    child_editors: e.properties || e.items
                }), this.item_info[s]
            }

            getElementEditor(t) {
                const e = this.getItemInfo(t);
                let s = this.getItemSchema(t);
                s = this.jsoneditor.expandRefs(s), s.title = `${e.title} ${t + 1}`;
                const i = this.jsoneditor.getEditorClass(s);
                let r;
                this.tabs_holder ? (r = "tabs-top" === this.schema.format ? this.theme.getTopTabContent() : this.theme.getTabContent(), r.id = `${this.path}.${t}`) : r = e.child_editors ? this.theme.getChildEditorHolder() : this.theme.getIndentedPanel(), this.row_holder.appendChild(r);
                const o = this.jsoneditor.createEditor(i, {
                    jsoneditor: this.jsoneditor,
                    schema: s,
                    container: r,
                    path: `${this.path}.${t}`,
                    parent: this,
                    required: !0
                });
                return o.preBuild(), o.build(), o.postBuild(), o.title_controls || (o.array_controls = this.theme.getButtonHolder(), r.appendChild(o.array_controls)), o
            }

            destroy() {
                this.empty(!0), this.title && this.title.parentNode && this.title.parentNode.removeChild(this.title), this.description && this.description.parentNode && this.description.parentNode.removeChild(this.description), this.row_holder && this.row_holder.parentNode && this.row_holder.parentNode.removeChild(this.row_holder), this.controls && this.controls.parentNode && this.controls.parentNode.removeChild(this.controls), this.panel && this.panel.parentNode && this.panel.parentNode.removeChild(this.panel), this.rows = this.row_cache = this.title = this.description = this.row_holder = this.panel = this.controls = null, super.destroy()
            }

            empty(t) {
                this.rows && (this.rows.forEach((e, s) => {
                    t && (e.tab && e.tab.parentNode && e.tab.parentNode.removeChild(e.tab), this.destroyRow(e, !0), this.row_cache[s] = null), this.rows[s] = null
                }), this.rows = [], t && (this.row_cache = []))
            }

            destroyRow(t, e) {
                const s = t.container;
                e ? (t.destroy(), s.parentNode && s.parentNode.removeChild(s), t.tab && t.tab.parentNode && t.tab.parentNode.removeChild(t.tab)) : (t.tab && (t.tab.style.display = "none"), s.style.display = "none", t.unregister())
            }

            getMax() {
                return Array.isArray(this.schema.items) && !1 === this.schema.additionalItems ? Math.min(this.schema.items.length, this.schema.maxItems || 1 / 0) : this.schema.maxItems || 1 / 0
            }

            refreshTabs(t) {
                this.rows.forEach(e => {
                    e.tab && (t ? e.tab_text.textContent = e.getHeaderText() : e.tab === this.active_tab ? this.theme.markTabActive(e) : this.theme.markTabInactive(e))
                })
            }

            setValue(t = [], e) {
                if (Array.isArray(t) || (t = [t]), JSON.stringify(t) === this.serialized) return;
                if (this.schema.minItems) for (; t.length < this.schema.minItems;) t.push(this.getItemInfo(t.length).default);
                this.getMax() && t.length > this.getMax() && (t = t.slice(0, this.getMax())), t.forEach((t, s) => {
                    if (this.rows[s]) this.rows[s].setValue(t, e); else if (this.row_cache[s]) this.rows[s] = this.row_cache[s], this.rows[s].setValue(t, e), this.rows[s].container.style.display = "", this.rows[s].tab && (this.rows[s].tab.style.display = ""), this.rows[s].register(), this.jsoneditor.trigger("addRow", this.rows[s]); else {
                        const s = this.addRow(t, e);
                        this.jsoneditor.trigger("addRow", s)
                    }
                });
                for (let e = t.length; e < this.rows.length; e++) this.destroyRow(this.rows[e]), this.rows[e] = null;
                this.rows = this.rows.slice(0, t.length);
                const s = this.rows.find(t => t.tab === this.active_tab);
                let i = void 0 !== s ? s.tab : null;
                !i && this.rows.length && (i = this.rows[0].tab), this.active_tab = i, this.refreshValue(e), this.refreshTabs(!0), this.refreshTabs(), this.onChange()
            }

            refreshValue(t) {
                const e = this.value ? this.value.length : 0;
                if (this.value = this.rows.map(t => t.getValue()), e !== this.value.length || t) {
                    const t = this.schema.minItems && this.schema.minItems >= this.rows.length;
                    this.rows.forEach((e, s) => {
                        e.movedown_button && (s === this.rows.length - 1 ? e.movedown_button.style.display = "none" : e.movedown_button.style.display = ""), e.delete_button && (e.delete_button.style.display = t ? "none" : ""), this.value[s] = e.getValue()
                    });
                    let e = !1;
                    this.value.length ? 1 === this.value.length ? (this.remove_all_rows_button.style.display = "none", t || this.hide_delete_last_row_buttons ? this.delete_last_row_button.style.display = "none" : (this.delete_last_row_button.style.display = "", e = !0)) : (t || this.hide_delete_last_row_buttons ? this.delete_last_row_button.style.display = "none" : (this.delete_last_row_button.style.display = "", e = !0), t || this.hide_delete_all_rows_buttons ? this.remove_all_rows_button.style.display = "none" : (this.remove_all_rows_button.style.display = "", e = !0)) : (this.delete_last_row_button.style.display = "none", this.remove_all_rows_button.style.display = "none"), this.getMax() && this.getMax() <= this.rows.length || this.hide_add_button ? this.add_row_button.style.display = "none" : (this.add_row_button.style.display = "", e = !0), !this.collapsed && e ? this.controls.style.display = "inline-block" : this.controls.style.display = "none"
                }
            }

            addRow(t, e) {
                const s = this.rows.length;
                this.rows[s] = this.getElementEditor(s), this.row_cache[s] = this.rows[s], this.tabs_holder && (this.rows[s].tab_text = document.createElement("span"), this.rows[s].tab_text.textContent = this.rows[s].getHeaderText(), "tabs-top" === this.schema.format ? (this.rows[s].tab = this.theme.getTopTab(this.rows[s].tab_text, this.getValidId(this.rows[s].path)), this.theme.addTopTab(this.tabs_holder, this.rows[s].tab)) : (this.rows[s].tab = this.theme.getTab(this.rows[s].tab_text, this.getValidId(this.rows[s].path)), this.theme.addTab(this.tabs_holder, this.rows[s].tab)), this.rows[s].tab.addEventListener("click", t => {
                    this.active_tab = this.rows[s].tab, this.refreshTabs(), t.preventDefault(), t.stopPropagation()
                }));
                const i = this.rows[s].title_controls || this.rows[s].array_controls;
                return this.hide_delete_buttons || (this.rows[s].delete_button = this.getButton(this.getItemTitle(), "delete", this.translate("button_delete_row_title", [this.getItemTitle()])), this.rows[s].delete_button.classList.add("delete", "json-editor-btntype-delete"), this.rows[s].delete_button.setAttribute("data-i", s), this.rows[s].delete_button.addEventListener("click", t => {
                    if (t.preventDefault(), t.stopPropagation(), !this.askConfirmation()) return !1;
                    const e = 1 * t.currentTarget.getAttribute("data-i"), s = this.getValue().filter((t, s) => s !== e);
                    let i = null;
                    const r = this.rows[e];
                    this.setValue(s), this.rows[e] ? i = this.rows[e].tab : this.rows[e - 1] && (i = this.rows[e - 1].tab), i && (this.active_tab = i, this.refreshTabs()), this.onChange(!0), this.jsoneditor.trigger("deleteRow", r)
                }), i && i.appendChild(this.rows[s].delete_button)), this.show_copy_button && (this.rows[s].copy_button = this.getButton(this.getItemTitle(), "copy", "Copy " + this.getItemTitle()), this.rows[s].copy_button.classList.add("copy", "json-editor-btntype-copy"), this.rows[s].copy_button.setAttribute("data-i", s), this.rows[s].copy_button.addEventListener("click", t => {
                    const e = this.getValue();
                    t.preventDefault(), t.stopPropagation();
                    const s = 1 * t.currentTarget.getAttribute("data-i");
                    e.forEach((t, i) => {
                        i === s && e.push(t)
                    }), this.setValue(e), this.refreshValue(!0), this.onChange(!0)
                }), i.appendChild(this.rows[s].copy_button)), s && !this.hide_move_buttons && (this.rows[s].moveup_button = this.getButton("", "tabs-top" === this.schema.format ? "moveleft" : "moveup", this.translate("button_move_up_title")), this.rows[s].moveup_button.classList.add("moveup", "json-editor-btntype-move"), this.rows[s].moveup_button.setAttribute("data-i", s), this.rows[s].moveup_button.addEventListener("click", t => {
                    t.preventDefault(), t.stopPropagation();
                    const e = 1 * t.currentTarget.getAttribute("data-i");
                    if (e <= 0) return;
                    const s = this.getValue(), i = s[e - 1];
                    s[e - 1] = s[e], s[e] = i, this.setValue(s), this.active_tab = this.rows[e - 1].tab, this.refreshTabs(), this.onChange(!0), this.jsoneditor.trigger("moveRow", this.rows[e - 1])
                }), i && i.appendChild(this.rows[s].moveup_button)), this.hide_move_buttons || (this.rows[s].movedown_button = this.getButton("", "tabs-top" === this.schema.format ? "moveright" : "movedown", this.translate("button_move_down_title")), this.rows[s].movedown_button.classList.add("movedown", "json-editor-btntype-move"), this.rows[s].movedown_button.setAttribute("data-i", s), this.rows[s].movedown_button.addEventListener("click", t => {
                    t.preventDefault(), t.stopPropagation();
                    const e = 1 * t.currentTarget.getAttribute("data-i"), s = this.getValue();
                    if (e >= s.length - 1) return;
                    const i = s[e + 1];
                    s[e + 1] = s[e], s[e] = i, this.setValue(s), this.active_tab = this.rows[e + 1].tab, this.refreshTabs(), this.onChange(!0), this.jsoneditor.trigger("moveRow", this.rows[e + 1])
                }), i && i.appendChild(this.rows[s].movedown_button)), t && this.rows[s].setValue(t, e), this.refreshTabs(), this.rows[s]
            }

            addControls() {
                this.collapsed = !1, this.toggle_button = this.getButton("", "collapse", this.translate("button_collapse")), this.toggle_button.classList.add("json-editor-btntype-toggle"), this.toggle_button.style.margin = "0 10px 0 0", this.title.insertBefore(this.toggle_button, this.title.childNodes[0]);
                const t = this.row_holder.style.display, e = this.controls.style.display;
                this.toggle_button.addEventListener("click", s => {
                    s.preventDefault(), s.stopPropagation(), this.collapsed ? (this.collapsed = !1, this.panel && (this.panel.style.display = ""), this.row_holder.style.display = t, this.tabs_holder && (this.tabs_holder.style.display = ""), this.controls.style.display = e, this.setButtonText(s.currentTarget, "", "collapse", this.translate("button_collapse"))) : (this.collapsed = !0, this.row_holder.style.display = "none", this.tabs_holder && (this.tabs_holder.style.display = "none"), this.controls.style.display = "none", this.panel && (this.panel.style.display = "none"), this.setButtonText(s.currentTarget, "", "expand", this.translate("button_expand")))
                }), this.options.collapsed && p(this.toggle_button, "click"), this.schema.options && void 0 !== this.schema.options.disable_collapse ? this.schema.options.disable_collapse && (this.toggle_button.style.display = "none") : this.jsoneditor.options.disable_collapse && (this.toggle_button.style.display = "none"), this.add_row_button = this.getButton(this.getItemTitle(), "add", this.translate("button_add_row_title", [this.getItemTitle()])), this.add_row_button.classList.add("json-editor-btntype-add"), this.add_row_button.addEventListener("click", t => {
                    t.preventDefault(), t.stopPropagation();
                    const e = this.rows.length;
                    let s;
                    this.row_cache[e] ? (s = this.rows[e] = this.row_cache[e], this.rows[e].setValue(this.rows[e].getDefault(), !0), this.rows[e].container.style.display = "", this.rows[e].tab && (this.rows[e].tab.style.display = ""), this.rows[e].register()) : s = this.addRow(), this.active_tab = this.rows[e].tab, this.refreshTabs(), this.refreshValue(), this.onChange(!0), this.jsoneditor.trigger("addRow", s)
                }), this.controls.appendChild(this.add_row_button), this.delete_last_row_button = this.getButton(this.translate("button_delete_last", [this.getItemTitle()]), "subtract", this.translate("button_delete_last_title", [this.getItemTitle()])), this.delete_last_row_button.classList.add("json-editor-btntype-deletelast"), this.delete_last_row_button.addEventListener("click", t => {
                    if (t.preventDefault(), t.stopPropagation(), !this.askConfirmation()) return !1;
                    const e = this.getValue();
                    let s = null;
                    const i = e.pop();
                    this.setValue(e), this.rows[this.rows.length - 1] && (s = this.rows[this.rows.length - 1].tab), s && (this.active_tab = s, this.refreshTabs()), this.onChange(!0), this.jsoneditor.trigger("deleteRow", i)
                }), this.controls.appendChild(this.delete_last_row_button), this.remove_all_rows_button = this.getButton(this.translate("button_delete_all"), "delete", this.translate("button_delete_all_title")), this.remove_all_rows_button.classList.add("json-editor-btntype-deleteall"), this.remove_all_rows_button.addEventListener("click", t => {
                    if (t.preventDefault(), t.stopPropagation(), !this.askConfirmation()) return !1;
                    this.empty(!0), this.setValue([]), this.onChange(!0), this.jsoneditor.trigger("deleteAllRows")
                }), this.controls.appendChild(this.remove_all_rows_button), this.tabs && (this.add_row_button.style.width = "100%", this.add_row_button.style.textAlign = "left", this.add_row_button.style.marginBottom = "3px", this.delete_last_row_button.style.width = "100%", this.delete_last_row_button.style.textAlign = "left", this.delete_last_row_button.style.marginBottom = "3px", this.remove_all_rows_button.style.width = "100%", this.remove_all_rows_button.style.textAlign = "left", this.remove_all_rows_button.style.marginBottom = "3px")
            }

            showValidationErrors(t) {
                const e = [], s = [];
                t.forEach(t => {
                    t.path === this.path ? e.push(t) : s.push(t)
                }), this.error_holder && (e.length ? (this.error_holder.innerHTML = "", this.error_holder.style.display = "", e.forEach(t => {
                    this.error_holder.appendChild(this.theme.getErrorMessage(t.message))
                })) : this.error_holder.style.display = "none"), this.rows.forEach(t => t.showValidationErrors(s))
            }
        }

        class v extends _ {
            onInputChange() {
                this.value = this.input.value, this.onChange(!0)
            }

            register() {
                super.register(), this.input && this.input.setAttribute("name", this.formname)
            }

            unregister() {
                super.unregister(), this.input && this.input.removeAttribute("name")
            }

            getNumColumns() {
                let t = this.getTitle().length;
                return Object.keys(this.select_values).forEach(e => t = Math.max(t, ("" + this.select_values[e]).length + 4)), Math.min(12, Math.max(t / 7, 2))
            }

            preBuild() {
                let t;
                super.preBuild(), this.select_options = {}, this.select_values = {}, this.option_keys = [], this.option_titles = [];
                const e = this.jsoneditor.expandRefs(this.schema.items || {}), s = e.enum || [],
                    i = e.options && e.options.enum_titles || [];
                for (t = 0; t < s.length; t++) this.sanitize(s[t]) === s[t] && (this.option_keys.push("" + s[t]), this.option_titles.push("" + (i[t] || s[t])), this.select_values["" + s[t]] = s[t])
            }

            build() {
                let t;
                if (this.options.compact || (this.header = this.label = this.theme.getFormInputLabel(this.getTitle(), this.isRequired())), this.schema.description && (this.description = this.theme.getFormInputDescription(this.schema.description)), this.options.infoText && (this.infoButton = this.theme.getInfoButton(this.options.infoText)), this.options.compact && this.container.classList.add("compact"), !this.schema.format && this.option_keys.length < 8 || "checkbox" === this.schema.format) {
                    for (this.input_type = "checkboxes", this.inputs = {}, this.controls = {}, t = 0; t < this.option_keys.length; t++) {
                        const e = this.formname + t.toString();
                        this.inputs[this.option_keys[t]] = this.theme.getCheckbox(), this.inputs[this.option_keys[t]].id = e, this.select_options[this.option_keys[t]] = this.inputs[this.option_keys[t]];
                        const s = this.theme.getCheckboxLabel(this.option_titles[t]);
                        s.htmlFor = e, this.controls[this.option_keys[t]] = this.theme.getFormControl(s, this.inputs[this.option_keys[t]])
                    }
                    this.control = this.theme.getMultiCheckboxHolder(this.controls, this.label, this.description, this.infoButton), this.inputs.controlgroup = this.inputs.controls = this.control
                } else {
                    for (this.input_type = "select", this.input = this.theme.getSelectInput(this.option_keys, !0), this.theme.setSelectOptions(this.input, this.option_keys, this.option_titles), this.input.setAttribute("multiple", "multiple"), this.input.size = Math.min(10, this.option_keys.length), t = 0; t < this.option_keys.length; t++) this.select_options[this.option_keys[t]] = this.input.children[t];
                    this.control = this.theme.getFormControl(this.label, this.input, this.description, this.infoButton)
                }
                (this.schema.readOnly || this.schema.readonly) && this.disable(!0), this.container.appendChild(this.control), this.multiselectChangeHandler = e => {
                    const s = [];
                    for (t = 0; t < this.option_keys.length; t++) this.select_options[this.option_keys[t]] && (this.select_options[this.option_keys[t]].selected || this.select_options[this.option_keys[t]].checked) && s.push(this.select_values[this.option_keys[t]]);
                    this.updateValue(s), this.onChange(!0)
                }, this.control.addEventListener("change", this.multiselectChangeHandler, !1), window.requestAnimationFrame(() => {
                    this.afterInputReady()
                })
            }

            postBuild() {
                super.postBuild()
            }

            afterInputReady() {
                this.theme.afterInputReady(this.input || this.inputs)
            }

            setValue(t, e) {
                t = t || [], Array.isArray(t) || (t = [t]), t = t.map(t => "" + t), Object.keys(this.select_options).forEach(e => {
                    this.select_options[e]["select" === this.input_type ? "selected" : "checked"] = t.includes(e)
                }), this.updateValue(t), this.onChange(!0)
            }

            removeValue(t) {
                t = [].concat(t), this.setValue(this.getValue().filter(e => !t.includes(e)))
            }

            addValue(t) {
                this.setValue(this.getValue().concat(t))
            }

            updateValue(t) {
                let e = !1;
                const s = [];
                for (let i = 0; i < t.length; i++) {
                    if (!this.select_options["" + t[i]]) {
                        e = !0;
                        continue
                    }
                    const r = this.sanitize(this.select_values[t[i]]);
                    s.push(r), r !== t[i] && (e = !0)
                }
                return this.value = s, e
            }

            sanitize(t) {
                return "boolean" === this.schema.items.type ? !!t : "number" === this.schema.items.type ? 1 * t || 0 : "integer" === this.schema.items.type ? Math.floor(1 * t || 0) : "" + t
            }

            enable() {
                this.always_disabled || (this.input ? this.input.disabled = !1 : this.inputs && Object.keys(this.inputs).forEach(t => this.inputs[t].disabled = !1), super.enable())
            }

            disable(t) {
                t && (this.always_disabled = !0), this.input ? this.input.disabled = !0 : this.inputs && Object.keys(this.inputs).forEach(t => this.inputs[t].disabled = !0), super.disable()
            }

            destroy() {
                super.destroy()
            }

            escapeRegExp(t) {
                return t.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")
            }

            showValidationErrors(t) {
                const e = new RegExp(`^${this.escapeRegExp(this.path)}(\\.\\d+)?$`),
                    s = t.reduce((t, s) => (s.path.match(e) && t.push(s.message), t), []);
                s.length ? this.theme.addInputError(this.input || this.inputs, s.join(". ") + ".") : this.theme.removeInputError(this.input || this.inputs)
            }
        }

        class C extends _ {
            constructor(t, e) {
                super(t, e), this.active = !1, this.parent && this.parent.schema && (Array.isArray(this.parent.schema.required) ? this.parent.schema.required.includes(this.key) || this.parent.schema.required.push(this.key) : this.parent.schema.required = [this.key])
            }

            build() {
                this.options.compact = !0;
                const t = this.schema.title || this.key, e = this.expandCallbacks("button", c({}, {
                    icon: "",
                    validated: !1,
                    align: "left",
                    action: (t, e) => {
                        window.alert(`No button action defined for "${t.path}"`)
                    }
                }, this.defaults.options.button || {}, this.options.button || {}));
                this.input = this.theme.getFormButton(t, e.icon, t), this.input.addEventListener("click", e.action, !1), (this.schema.readOnly || this.schema.readonly || this.schema.template) && (this.always_disabled = !0, this.input.setAttribute("readonly", "true")), this.setInputAttributes(["readonly"]), this.control = this.theme.getFormButtonHolder(e.align), this.control.appendChild(this.input), this.container.appendChild(this.control), this.changeHandler = () => {
                    this.jsoneditor.validate(this.jsoneditor.getValue()).length > 0 ? this.disable() : this.enable()
                }, e.validated && this.jsoneditor.on("change", this.changeHandler)
            }

            enable() {
                this.always_disabled || (this.input.disabled = !1, super.enable())
            }

            disable(t) {
                t && (this.always_disabled = !0), this.input.disabled = !0, super.disable()
            }

            getNumColumns() {
                return 2
            }

            activate() {
                this.active = !1, this.enable()
            }

            deactivate() {
                this.isRequired() || (this.active = !1, this.disable())
            }

            destroy() {
                this.jsoneditor.off("change", this.changeHandler), this.changeHandler = null, super.destroy()
            }
        }

        class x extends _ {
            setValue(t, e) {
                let s = this.typecast(t);
                const i = !!this.jsoneditor.options.use_default_values || void 0 !== this.schema.default;
                this.enum_values.includes(s) && (!e || this.isRequired() || i) || (s = this.enum_values[0]), this.value !== s && (e ? this.is_dirty = !1 : "change" === this.jsoneditor.options.show_errors && (this.is_dirty = !0), this.input.value = this.enum_options[this.enum_values.indexOf(s)], this.value = s, this.onChange(), this.change())
            }

            register() {
                super.register(), this.input && this.input.setAttribute("name", this.formname)
            }

            unregister() {
                super.unregister(), this.input && this.input.removeAttribute("name")
            }

            getNumColumns() {
                if (!this.enum_options) return 3;
                let t = this.getTitle().length;
                for (let e = 0; e < this.enum_options.length; e++) t = Math.max(t, this.enum_options[e].length + 4);
                return Math.min(12, Math.max(t / 7, 2))
            }

            typecast(t) {
                return "boolean" === this.schema.type ? "undefined" === t || void 0 === t ? void 0 : !!t : "number" === this.schema.type ? 1 * t || 0 : "integer" === this.schema.type ? Math.floor(1 * t || 0) : this.schema.enum && void 0 === t ? void 0 : "" + t
            }

            getValue() {
                if (this.dependenciesFulfilled) return this.typecast(this.value)
            }

            preBuild() {
                let t, e;
                if (this.input_type = "select", this.enum_options = [], this.enum_values = [], this.enum_display = [], this.schema.enum) {
                    const t = this.schema.options && this.schema.options.enum_titles || [];
                    this.schema.enum.forEach((e, s) => {
                        this.enum_options[s] = "" + e, this.enum_display[s] = "" + (t[s] || e), this.enum_values[s] = this.typecast(e)
                    }), this.isRequired() || (this.enum_display.unshift(" "), this.enum_options.unshift("undefined"), this.enum_values.unshift(void 0))
                } else if ("boolean" === this.schema.type) this.enum_display = this.schema.options && this.schema.options.enum_titles || ["true", "false"], this.enum_options = ["1", ""], this.enum_values = [!0, !1], this.isRequired() || (this.enum_display.unshift(" "), this.enum_options.unshift("undefined"), this.enum_values.unshift(void 0)); else {
                    if (!this.schema.enumSource) throw new Error("'select' editor requires the enum property to be set.");
                    if (this.enumSource = [], this.enum_display = [], this.enum_options = [], this.enum_values = [], Array.isArray(this.schema.enumSource)) for (t = 0; t < this.schema.enumSource.length; t++) "string" == typeof this.schema.enumSource[t] ? this.enumSource[t] = {source: this.schema.enumSource[t]} : Array.isArray(this.schema.enumSource[t]) ? this.enumSource[t] = this.schema.enumSource[t] : this.enumSource[t] = c({}, this.schema.enumSource[t]); else this.schema.enumValue ? this.enumSource = [{
                        source: this.schema.enumSource,
                        value: this.schema.enumValue
                    }] : this.enumSource = [{source: this.schema.enumSource}];
                    for (t = 0; t < this.enumSource.length; t++) this.enumSource[t].value && (e = this.expandCallbacks("template", {template: this.enumSource[t].value}), "function" == typeof e.template ? this.enumSource[t].value = e.template : this.enumSource[t].value = this.jsoneditor.compileTemplate(this.enumSource[t].value, this.template_engine)), this.enumSource[t].title && (e = this.expandCallbacks("template", {template: this.enumSource[t].title}), "function" == typeof e.template ? this.enumSource[t].title = e.template : this.enumSource[t].title = this.jsoneditor.compileTemplate(this.enumSource[t].title, this.template_engine)), this.enumSource[t].filter && this.enumSource[t].value && (e = this.expandCallbacks("template", {template: this.enumSource[t].filter}), "function" == typeof e.template ? this.enumSource[t].filter = e.template : this.enumSource[t].filter = this.jsoneditor.compileTemplate(this.enumSource[t].filter, this.template_engine))
                }
            }

            build() {
                this.options.compact || (this.header = this.label = this.theme.getFormInputLabel(this.getTitle(), this.isRequired())), this.schema.description && (this.description = this.theme.getFormInputDescription(this.schema.description)), this.options.infoText && (this.infoButton = this.theme.getInfoButton(this.options.infoText)), this.options.compact && this.container.classList.add("compact"), this.input = this.theme.getSelectInput(this.enum_options, !1), this.theme.setSelectOptions(this.input, this.enum_options, this.enum_display), (this.schema.readOnly || this.schema.readonly) && (this.always_disabled = !0, this.input.disabled = !0), this.setInputAttributes([]), this.input.addEventListener("change", t => {
                    t.preventDefault(), t.stopPropagation(), this.onInputChange()
                }), this.control = this.theme.getFormControl(this.label, this.input, this.description, this.infoButton), this.container.appendChild(this.control), this.value = this.enum_values[0], window.requestAnimationFrame(() => {
                    this.input.parentNode && this.afterInputReady()
                })
            }

            afterInputReady() {
                this.theme.afterInputReady(this.input)
            }

            onInputChange() {
                const t = this.typecast(this.input.value);
                let e;
                e = this.enum_values.includes(t) ? this.enum_values[this.enum_values.indexOf(t)] : this.enum_values[0], e !== this.value && (this.is_dirty = !0, this.value = e, this.onChange(!0))
            }

            onWatchedFieldChange() {
                let t, e, s = [], i = [];
                if (this.enumSource) {
                    t = this.getWatchedFieldValues();
                    for (let r = 0; r < this.enumSource.length; r++) if (Array.isArray(this.enumSource[r])) s = s.concat(this.enumSource[r]), i = i.concat(this.enumSource[r]); else {
                        let o = [];
                        if (o = Array.isArray(this.enumSource[r].source) ? this.enumSource[r].source : t[this.enumSource[r].source], o) {
                            if (this.enumSource[r].slice && (o = Array.prototype.slice.apply(o, this.enumSource[r].slice)), this.enumSource[r].filter) {
                                const s = [];
                                for (e = 0; e < o.length; e++) this.enumSource[r].filter({
                                    i: e,
                                    item: o[e],
                                    watched: t
                                }) && s.push(o[e]);
                                o = s
                            }
                            const n = [], a = [];
                            for (e = 0; e < o.length; e++) {
                                const t = o[e];
                                this.enumSource[r].value ? a[e] = this.typecast(this.enumSource[r].value({
                                    i: e,
                                    item: t
                                })) : a[e] = o[e], this.enumSource[r].title ? n[e] = this.enumSource[r].title({
                                    i: e,
                                    item: t
                                }) : n[e] = a[e]
                            }
                            this.enumSource[r].sort && ((t, e, s) => {
                                t.map((t, s) => ({
                                    v: t,
                                    t: e[s]
                                })).sort((t, e) => t.v < e.v ? -s : t.v === e.v ? 0 : s).forEach((s, i) => {
                                    t[i] = s.v, e[i] = s.t
                                })
                            }).bind(null, a, n, "desc" === this.enumSource[r].sort ? 1 : -1)(), s = s.concat(a), i = i.concat(n)
                        }
                    }
                    const r = this.value;
                    this.theme.setSelectOptions(this.input, s, i), this.enum_options = s, this.enum_display = i, this.enum_values = s, s.includes(r) || !1 !== this.jsoneditor.options.enum_source_value_auto_select ? (this.input.value = r, this.value = r) : (this.input.value = s[0], this.value = this.typecast(s[0] || ""), this.parent && !this.watchLoop ? this.parent.onChildEditorChange(this) : this.jsoneditor.onChange(), this.jsoneditor.notifyWatchers(this.path))
                }
                super.onWatchedFieldChange()
            }

            enable() {
                this.always_disabled || (this.input.disabled = !1), super.enable()
            }

            disable(t) {
                t && (this.always_disabled = !0), this.input.disabled = !0, super.disable(t)
            }

            destroy() {
                this.label && this.label.parentNode && this.label.parentNode.removeChild(this.label), this.description && this.description.parentNode && this.description.parentNode.removeChild(this.description), this.input && this.input.parentNode && this.input.parentNode.removeChild(this.input), super.destroy()
            }

            showValidationErrors(t) {
                this.previous_error_setting = this.jsoneditor.options.show_errors;
                const e = t.reduce((t, e) => (e.path === this.path && t.push(e.message), t), []);
                e.length ? this.theme.addInputError(this.input, e.join(". ") + ".") : this.theme.removeInputError(this.input)
            }
        }

        class k extends x {
            setValue(t, e) {
                if (this.choices_instance) {
                    let s = this.typecast(t || "");
                    if (this.enum_values.includes(s) || (s = this.enum_values[0]), this.value === s) return;
                    e ? this.is_dirty = !1 : "change" === this.jsoneditor.options.show_errors && (this.is_dirty = !0), this.input.value = this.enum_options[this.enum_values.indexOf(s)], this.choices_instance.setChoiceByValue(this.input.value), this.value = s, this.onChange()
                } else super.setValue(t, e)
            }

            afterInputReady() {
                if (window.Choices && !this.choices_instance) {
                    const t = this.expandCallbacks("choices", c({}, this.defaults.options.choices || {}, this.options.choices || {}));
                    this.choices_instance = new window.Choices(this.input, t)
                }
                super.afterInputReady()
            }

            onWatchedFieldChange() {
                if (super.onWatchedFieldChange(), this.choices_instance) {
                    const t = this.enum_options.map((t, e) => ({value: t, label: this.enum_display[e]}));
                    this.choices_instance.setChoices(t, "value", "label", !0), this.choices_instance.setChoiceByValue("" + this.value)
                }
            }

            enable() {
                !this.always_disabled && this.choices_instance && this.choices_instance.enable(), super.enable()
            }

            disable(t) {
                this.choices_instance && this.choices_instance.disable(), super.disable(t)
            }

            destroy() {
                this.choices_instance && (this.choices_instance.destroy(), this.choices_instance = null), super.destroy()
            }
        }

        k.rules = {".choices > *": "box-sizing:border-box"};

        class E extends y {
            build() {
                if (super.build(), void 0 !== this.schema.minimum) {
                    let {minimum: t} = this.schema;
                    void 0 !== this.schema.exclusiveMinimum && (t += 1), this.input.setAttribute("min", t)
                }
                if (void 0 !== this.schema.maximum) {
                    let {maximum: t} = this.schema;
                    void 0 !== this.schema.exclusiveMaximum && (t -= 1), this.input.setAttribute("max", t)
                }
                if (void 0 !== this.schema.step) {
                    const t = this.schema.step || 1;
                    this.input.setAttribute("step", t)
                }
                this.setInputAttributes(["maxlength", "pattern", "readonly", "min", "max", "step"])
            }

            getNumColumns() {
                return 2
            }

            getValue() {
                if (this.dependenciesFulfilled) return function (t) {
                    if (null == t) return !1;
                    const e = t.match(m), s = parseFloat(t);
                    return null !== e && !isNaN(s) && isFinite(s)
                }(this.value) ? parseFloat(this.value) : this.value
            }
        }

        class j extends y {
            build() {
                if (this.options.compact || (this.header = this.label = this.theme.getFormInputLabel(this.getTitle(), this.isRequired())), this.schema.description && (this.description = this.theme.getFormInputDescription(this.schema.description)), this.options.infoText && (this.infoButton = this.theme.getInfoButton(this.options.infoText)), this.options.compact && this.container.classList.add("compact"), this.ratingContainer = document.createElement("div"), this.ratingContainer.classList.add("starrating"), void 0 === this.schema.enum) {
                    let t = this.schema.maximum ? this.schema.maximum : 5;
                    this.schema.exclusiveMaximum && t--, this.enum_values = [];
                    for (let e = 0; e < t; e++) this.enum_values.push(e + 1)
                } else this.enum_values = this.schema.enum;
                this.radioGroup = [];
                const t = t => {
                    t.preventDefault(), t.stopPropagation(), this.setValue(t.currentTarget.value), this.onChange(!0)
                };
                for (let e = this.enum_values.length - 1; e > -1; e--) {
                    const s = this.formname + (e + 1), i = this.theme.getFormInputField("radio");
                    i.name = this.formname + "[starrating]", i.value = this.enum_values[e], i.id = s, i.addEventListener("change", t, !1), this.radioGroup.push(i);
                    const r = document.createElement("label");
                    r.htmlFor = s, r.title = this.enum_values[e], this.options.displayValue && r.classList.add("starrating-display-enabled"), this.ratingContainer.appendChild(i), this.ratingContainer.appendChild(r)
                }
                if (this.options.displayValue && (this.displayRating = document.createElement("div"), this.displayRating.classList.add("starrating-display"), this.displayRating.innerText = this.enum_values[0], this.ratingContainer.appendChild(this.displayRating)), this.schema.readOnly || this.schema.readonly) {
                    this.always_disabled = !0;
                    for (let t = 0; t < this.radioGroup.length; t++) this.radioGroup[t].disabled = !0;
                    this.ratingContainer.classList.add("readonly")
                }
                const e = this.theme.getContainer();
                e.appendChild(this.ratingContainer), this.input = e, this.control = this.theme.getFormControl(this.label, e, this.description, this.infoButton), this.container.appendChild(this.control), this.refreshValue()
            }

            enable() {
                if (!this.always_disabled) {
                    for (let t = 0; t < this.radioGroup.length; t++) this.radioGroup[t].disabled = !1;
                    this.ratingContainer.classList.remove("readonly"), super.enable()
                }
            }

            disable(t) {
                t && (this.always_disabled = !0);
                for (let t = 0; t < this.radioGroup.length; t++) this.radioGroup[t].disabled = !0;
                this.ratingContainer.classList.add("readonly"), super.disable()
            }

            destroy() {
                this.ratingContainer.parentNode && this.ratingContainer.parentNode.parentNode && this.ratingContainer.parentNode.parentNode.removeChild(this.ratingContainer.parentNode), this.label && this.label.parentNode && this.label.parentNode.removeChild(this.label), this.description && this.description.parentNode && this.description.parentNode.removeChild(this.description), super.destroy()
            }

            getNumColumns() {
                return 2
            }

            getValue() {
                if (this.dependenciesFulfilled) return "integer" === this.schema.type ? "" === this.value ? void 0 : 1 * this.value : this.value
            }

            setValue(t) {
                for (let e = 0; e < this.radioGroup.length; e++) if (this.radioGroup[e].value === "" + t) {
                    this.radioGroup[e].checked = !0, this.value = t, this.options.displayValue && (this.displayRating.innerHTML = this.value), this.onChange(!0);
                    break
                }
            }
        }

        j.rules = {
            ".starrating": "direction:rtl;display:inline-block;white-space:nowrap",
            ".starrating > input": "display:none",
            ".starrating > label:before": "content:'%5C2606';margin:1px;font-size:18px;font-style:normal;font-weight:400;line-height:1;font-family:'Arial';display:inline-block",
            ".starrating > label": "color:%23888;cursor:pointer;margin:8px%200%202px%200",
            ".starrating > label.starrating-display-enabled": "margin:1px%200%200%200",
            ".starrating > input:checked ~ label": "color:%23ffca08",
            ".starrating:not(.readonly) > input:hover ~ label": "color:%23ffca08",
            ".starrating > input:checked ~ label:before": "content:'%5C2605';text-shadow:0%200%201px%20rgba(0%2C20%2C20%2C1)",
            ".starrating:not(.readonly) > input:hover ~ label:before": "content:'%5C2605';text-shadow:0%200%201px%20rgba(0%2C20%2C20%2C1)",
            ".starrating .starrating-display": "position:relative;direction:rtl;text-align:center;font-size:10px;line-height:0px"
        };
        const L = {
            ace: class extends y {
                setValue(t, e, s) {
                    const i = super.setValue(t, e, s);
                    void 0 !== i && i.changed && this.ace_editor_instance && (this.ace_editor_instance.setValue(i.value), this.ace_editor_instance.session.getSelection().clearSelection(), this.ace_editor_instance.resize())
                }

                build() {
                    this.options.format = "textarea", super.build(), this.input_type = this.schema.format, this.input.setAttribute("data-schemaformat", this.input_type)
                }

                afterInputReady() {
                    let t;
                    if (window.ace) {
                        let e = this.input_type;
                        "cpp" !== e && "c++" !== e && "c" !== e || (e = "c_cpp"), t = this.expandCallbacks("ace", c({}, {
                            selectionStyle: "text",
                            minLines: 30,
                            maxLines: 30
                        }, this.defaults.options.ace || {}, this.options.ace || {}, {mode: "ace/mode/" + e})), this.ace_container = document.createElement("div"), this.ace_container.style.width = "100%", this.ace_container.style.position = "relative", this.input.parentNode.insertBefore(this.ace_container, this.input), this.input.style.display = "none", this.ace_editor_instance = window.ace.edit(this.ace_container, t), this.ace_editor_instance.setValue(this.getValue()), this.ace_editor_instance.session.getSelection().clearSelection(), this.ace_editor_instance.resize(), (this.schema.readOnly || this.schema.readonly || this.schema.template) && this.ace_editor_instance.setReadOnly(!0), this.ace_editor_instance.on("change", () => {
                            this.input.value = this.ace_editor_instance.getValue(), this.refreshValue(), this.is_dirty = !0, this.onChange(!0)
                        }), this.theme.afterInputReady(this.input)
                    } else super.afterInputReady()
                }

                getNumColumns() {
                    return 6
                }

                enable() {
                    !this.always_disabled && this.ace_editor_instance && this.ace_editor_instance.setReadOnly(!1), super.enable()
                }

                disable(t) {
                    this.ace_editor_instance && this.ace_editor_instance.setReadOnly(!0), super.disable(t)
                }

                destroy() {
                    this.ace_editor_instance && (this.ace_editor_instance.destroy(), this.ace_editor_instance = null), super.destroy()
                }
            }, array: w, arrayChoices: class extends v {
                setValue(t, e) {
                    this.choices_instance ? (t = [].concat(t).map(t => "" + t), this.updateValue(t), this.choices_instance.removeActiveItems(), this.choices_instance.setChoiceByValue(this.value), this.onChange(!0)) : super.setValue(t, e)
                }

                afterInputReady() {
                    if (window.Choices && !this.choices_instance) {
                        const t = this.expandCallbacks("choices", c({}, {
                            removeItems: !0,
                            removeItemButton: !0
                        }, this.defaults.options.choices || {}, this.options.choices || {}, {
                            addItems: !0,
                            editItems: !1,
                            duplicateItemsAllowed: !1
                        }));
                        this.newEnumAllowed = !1, this.choices_instance = new window.Choices(this.input, t), this.control.removeEventListener("change", this.multiselectChangeHandler), this.multiselectChangeHandler = t => {
                            const e = this.choices_instance.getValue(!0);
                            this.updateValue(e), this.onChange(!0)
                        }, this.control.addEventListener("change", this.multiselectChangeHandler, !1)
                    }
                    super.afterInputReady()
                }

                updateValue(t) {
                    t = [].concat(t);
                    let e = !1;
                    const s = [];
                    for (let i = 0; i < t.length; i++) {
                        if (!this.select_values["" + t[i]]) {
                            if (e = !0, !this.newEnumAllowed) continue;
                            if (!this.addNewOption(t[i])) continue
                        }
                        const r = this.sanitize(this.select_values[t[i]]);
                        s.push(r), r !== t[i] && (e = !0)
                    }
                    return this.value = s, e
                }

                addNewOption(t) {
                    return this.option_keys.push("" + t), this.option_titles.push("" + t), this.select_values["" + t] = t, this.schema.items.enum.push(t), this.choices_instance.setChoices([{
                        value: "" + t,
                        label: "" + t
                    }], "value", "label", !1), !0
                }

                enable() {
                    !this.always_disabled && this.choices_instance && this.choices_instance.enable(), super.enable()
                }

                disable(t) {
                    this.choices_instance && this.choices_instance.disable(), super.disable(t)
                }

                destroy() {
                    this.choices_instance && (this.choices_instance.destroy(), this.choices_instance = null), super.destroy()
                }
            }, arraySelect2: class extends v {
                setValue(t, e) {
                    this.select2_instance ? (t = [].concat(t).map(t => "" + t), this.updateValue(t), this.select2v4 ? this.select2_instance.val(this.value).change() : this.select2_instance.select2("val", this.value), this.onChange(!0)) : super.setValue(t, e)
                }

                afterInputReady() {
                    let t;
                    window.jQuery && window.jQuery.fn && window.jQuery.fn.select2 && !this.select2_instance && (t = this.expandCallbacks("select2", c({}, {
                        tags: !0,
                        width: "100%"
                    }, this.defaults.options.select2 || {}, this.options.select2 || {})), this.newEnumAllowed = t.tags = !!t.tags && this.schema.items && "string" === this.schema.items.type, this.select2_instance = window.jQuery(this.input).select2(t), this.select2v4 = u(this.select2_instance.select2, "amd"), this.selectChangeHandler = () => {
                        const t = this.select2v4 ? this.select2_instance.val() : this.select2_instance.select2("val");
                        this.updateValue(t), this.onChange(!0)
                    }, this.select2_instance.on("select2-blur", this.selectChangeHandler), this.select2_instance.on("change", this.selectChangeHandler)), super.afterInputReady()
                }

                updateValue(t) {
                    t = [].concat(t);
                    let e = !1;
                    const s = [];
                    for (let i = 0; i < t.length; i++) {
                        if (!this.select_values["" + t[i]]) {
                            if (e = !0, !this.newEnumAllowed) continue;
                            if (!this.addNewOption(t[i])) continue
                        }
                        const r = this.sanitize(this.select_values[t[i]]);
                        s.push(r), r !== t[i] && (e = !0)
                    }
                    return this.value = s, e
                }

                addNewOption(t) {
                    this.option_keys.push("" + t), this.option_titles.push("" + t), this.select_values["" + t] = t, this.schema.items.enum.push(t);
                    const e = this.input.querySelector(`option[value="${t}"]`);
                    return e ? e.removeAttribute("data-select2-tag") : this.input.appendChild(new Option(t, t, !1, !1)).trigger("change"), !0
                }

                enable() {
                    !this.always_disabled && this.select2_instance && (this.select2v4 ? this.select2_instance.prop("disabled", !1) : this.select2_instance.select2("enable", !0)), super.enable()
                }

                disable(t) {
                    this.select2_instance && (this.select2v4 ? this.select2_instance.prop("disabled", !0) : this.select2_instance.select2("enable", !1)), super.disable()
                }

                destroy() {
                    this.select2_instance && (this.select2_instance.select2("destroy"), this.select2_instance = null), super.destroy()
                }
            }, arraySelectize: class extends v {
                setValue(t, e) {
                    this.selectize_instance ? (t = [].concat(t).map(t => "" + t), this.updateValue(t), this.selectize_instance.setValue(this.value), this.onChange(!0)) : super.setValue(t, e)
                }

                afterInputReady() {
                    let t;
                    window.jQuery && window.jQuery.fn && window.jQuery.fn.selectize && !this.selectize_instance && (t = this.expandCallbacks("selectize", c({}, {
                        plugins: ["remove_button"],
                        delimiter: !1,
                        createOnBlur: !0,
                        create: !0
                    }, this.defaults.options.selectize || {}, this.options.selectize || {})), this.newEnumAllowed = t.create = !!t.create && this.schema.items && "string" === this.schema.items.type, this.selectize_instance = window.jQuery(this.input).selectize(t)[0].selectize, this.control.removeEventListener("change", this.multiselectChangeHandler), this.multiselectChangeHandler = t => {
                        const e = this.selectize_instance.getValue();
                        this.updateValue(e), this.onChange(!0)
                    }, this.selectize_instance.on("change", this.multiselectChangeHandler)), super.afterInputReady()
                }

                updateValue(t) {
                    t = [].concat(t);
                    let e = !1;
                    const s = [];
                    for (let i = 0; i < t.length; i++) {
                        if (!this.select_values["" + t[i]]) {
                            if (e = !0, !this.newEnumAllowed) continue;
                            if (!this.addNewOption(t[i])) continue
                        }
                        const r = this.sanitize(this.select_values[t[i]]);
                        s.push(r), r !== t[i] && (e = !0)
                    }
                    return this.value = s, e
                }

                addNewOption(t) {
                    return this.option_keys.push("" + t), this.option_titles.push("" + t), this.select_values["" + t] = t, this.schema.items.enum.push(t), this.selectize_instance.addOption({
                        text: t,
                        value: t
                    }), !0
                }

                enable() {
                    !this.always_disabled && this.selectize_instance && this.selectize_instance.unlock(), super.enable()
                }

                disable(t) {
                    this.selectize_instance && this.selectize_instance.lock(), super.disable(t)
                }

                destroy() {
                    this.selectize_instance && (this.selectize_instance.destroy(), this.selectize_instance = null), super.destroy()
                }
            }, autocomplete: class extends y {
                postBuild() {
                    window.Autocomplete && (this.autocomplete_wrapper = document.createElement("div"), this.input.parentNode.insertBefore(this.autocomplete_wrapper, this.input.nextSibling), this.autocomplete_wrapper.appendChild(this.input), this.autocomplete_dropdown = document.createElement("ul"), this.input.parentNode.insertBefore(this.autocomplete_dropdown, this.input.nextSibling)), super.postBuild()
                }

                afterInputReady() {
                    let t;
                    window.Autocomplete && !this.autocomplete_instance && (t = this.expandCallbacks("autocomplete", c({}, {
                        search: (t, e) => (console.log(`No "search" callback defined for autocomplete in property "${t.key}"`), []),
                        baseClass: "autocomplete"
                    }, this.defaults.options.autocomplete || {}, this.options.autocomplete || {})), this.autocomplete_wrapper.classList.add(t.baseClass), this.autocomplete_dropdown.classList.add(t.baseClass + "-result-list"), this.autocomplete_instance = new window.Autocomplete(this.autocomplete_wrapper, t)), super.afterInputReady()
                }

                destroy() {
                    this.autocomplete_instance && (this.input && this.input.parentNode && this.input.parentNode.removeChild(this.input), this.autocomplete_dropdown && this.autocomplete_dropdown.parentNode && this.autocomplete_dropdown.parentNode.removeChild(this.autocomplete_dropdown), this.autocomplete_wrapper && this.autocomplete_wrapper.parentNode && this.autocomplete_wrapper.parentNode.removeChild(this.autocomplete_wrapper), this.autocomplete_instance = null), super.destroy()
                }
            }, base64: class extends _ {
                getNumColumns() {
                    return 4
                }

                setFileReaderListener(t) {
                    t.addEventListener("load", t => {
                        if (this.count === this.current_item_index) this.value[this.count][this.key] = t.target.result; else {
                            const e = {};
                            for (const t in this.parent.schema.properties) e[t] = "";
                            e[this.key] = t.target.result, this.value.splice(this.count, 0, e)
                        }
                        this.count += 1, this.count === this.total + this.current_item_index && this.arrayEditor.setValue(this.value)
                    })
                }

                build() {
                    if (this.title = this.header = this.label = this.theme.getFormInputLabel(this.getTitle(), this.isRequired()), this.options.infoText && (this.infoButton = this.theme.getInfoButton(this.options.infoText)), this.input = this.theme.getFormInputField("hidden"), this.container.appendChild(this.input), !this.schema.readOnly && !this.schema.readonly) {
                        if (!window.FileReader) throw new Error("FileReader required for base64 editor");
                        this.uploader = this.theme.getFormInputField("file"), this.schema.options && this.schema.options.multiple && !0 === this.schema.options.multiple && this.parent && "object" === this.parent.schema.type && this.parent.parent && "array" === this.parent.parent.schema.type && this.uploader.setAttribute("multiple", ""), this.uploader.addEventListener("change", t => {
                            if (t.preventDefault(), t.stopPropagation(), t.currentTarget.files && t.currentTarget.files.length) if (t.currentTarget.files.length > 1 && this.schema.options && this.schema.options.multiple && !0 === this.schema.options.multiple && this.parent && "object" === this.parent.schema.type && this.parent.parent && "array" === this.parent.parent.schema.type) {
                                this.arrayEditor = this.jsoneditor.getEditor(this.parent.parent.path), this.value = this.arrayEditor.getValue(), this.total = t.currentTarget.files.length, this.current_item_index = parseInt(this.parent.key), this.count = this.current_item_index;
                                for (let e = 0; e < this.total; e++) {
                                    const s = new FileReader;
                                    this.setFileReaderListener(s), s.readAsDataURL(t.currentTarget.files[e])
                                }
                            } else {
                                let e = new FileReader;
                                e.onload = t => {
                                    this.value = t.target.result, this.refreshPreview(), this.onChange(!0), e = null
                                }, e.readAsDataURL(t.currentTarget.files[0])
                            }
                        })
                    }
                    this.preview = this.theme.getFormInputDescription(this.schema.description), this.container.appendChild(this.preview), this.control = this.theme.getFormControl(this.label, this.uploader || this.input, this.preview, this.infoButton), this.container.appendChild(this.control)
                }

                refreshPreview() {
                    if (this.last_preview === this.value) return;
                    if (this.last_preview = this.value, this.preview.innerHTML = "", !this.value) return;
                    let t = this.value.match(/^data:([^;,]+)[;,]/);
                    if (t && (t = t[1]), t) {
                        if (this.preview.innerHTML = `<strong>Type:</strong> ${t}, <strong>Size:</strong> ${Math.floor((this.value.length - this.value.split(",")[0].length - 1) / 1.33333)} bytes`, "image" === t.substr(0, 5)) {
                            this.preview.innerHTML += "<br>";
                            const t = document.createElement("img");
                            t.style.maxWidth = "100%", t.style.maxHeight = "100px", t.src = this.value, this.preview.appendChild(t)
                        }
                    } else this.preview.innerHTML = "<em>Invalid data URI</em>"
                }

                enable() {
                    this.always_disabled || (this.uploader && (this.uploader.disabled = !1), super.enable())
                }

                disable(t) {
                    t && (this.always_disabled = !0), this.uploader && (this.uploader.disabled = !0), super.disable()
                }

                setValue(t) {
                    this.value !== t && (this.value = t, this.input.value = this.value, this.refreshPreview(), this.onChange())
                }

                destroy() {
                    this.preview && this.preview.parentNode && this.preview.parentNode.removeChild(this.preview), this.title && this.title.parentNode && this.title.parentNode.removeChild(this.title), this.input && this.input.parentNode && this.input.parentNode.removeChild(this.input), this.uploader && this.uploader.parentNode && this.uploader.parentNode.removeChild(this.uploader), super.destroy()
                }
            }, button: C, checkbox: class extends _ {
                setValue(t, e) {
                    t = !!t;
                    const s = this.getValue() !== t;
                    this.value = t, this.input.checked = this.value, this.onChange(s)
                }

                register() {
                    super.register(), this.input && this.input.setAttribute("name", this.formname)
                }

                unregister() {
                    super.unregister(), this.input && this.input.removeAttribute("name")
                }

                getNumColumns() {
                    return Math.min(12, Math.max(this.getTitle().length / 7, 2))
                }

                build() {
                    this.parent.options.table_row || (this.label = this.header = this.theme.getCheckboxLabel(this.getTitle(), this.isRequired()), this.label.htmlFor = this.formname), this.schema.description && (this.description = this.theme.getFormInputDescription(this.schema.description)), this.options.infoText && !this.options.compact && (this.infoButton = this.theme.getInfoButton(this.options.infoText)), this.options.compact && this.container.classList.add("compact"), this.input = this.theme.getCheckbox(), this.input.id = this.formname, this.control = this.theme.getFormControl(this.label, this.input, this.description, this.infoButton), (this.schema.readOnly || this.schema.readonly) && (this.always_disabled = !0, this.input.disabled = !0), this.input.addEventListener("change", t => {
                        t.preventDefault(), t.stopPropagation(), this.value = t.currentTarget.checked, this.onChange(!0)
                    }), this.container.appendChild(this.control)
                }

                enable() {
                    this.always_disabled || (this.input.disabled = !1, super.enable())
                }

                disable(t) {
                    t && (this.always_disabled = !0), this.input.disabled = !0, super.disable()
                }

                destroy() {
                    this.label && this.label.parentNode && this.label.parentNode.removeChild(this.label), this.description && this.description.parentNode && this.description.parentNode.removeChild(this.description), this.input && this.input.parentNode && this.input.parentNode.removeChild(this.input), super.destroy()
                }

                showValidationErrors(t) {
                    if ("always" === this.jsoneditor.options.show_errors) ; else if (!this.is_dirty && this.previous_error_setting === this.jsoneditor.options.show_errors) return;
                    this.previous_error_setting = this.jsoneditor.options.show_errors;
                    const e = t.reduce((t, e) => (e.path === this.path && t.push(e.message), t), []);
                    this.input.controlgroup = this.control, e.length ? this.theme.addInputError(this.input, e.join(". ") + ".") : this.theme.removeInputError(this.input)
                }
            }, choices: k, datetime: class extends y {
                build() {
                    if (super.build(), this.input && window.flatpickr && "object" == typeof this.options.flatpickr) {
                        this.options.flatpickr.enableTime = "date" !== this.schema.format, this.options.flatpickr.noCalendar = "time" === this.schema.format, "integer" === this.schema.type && (this.options.flatpickr.mode = "single"), this.input.setAttribute("data-input", "");
                        let {input: t} = this;
                        if (!0 === this.options.flatpickr.wrap) {
                            const e = [];
                            if (!1 !== this.options.flatpickr.showToggleButton) {
                                const t = this.getButton("", "time" === this.schema.format ? "time" : "calendar", this.translate("flatpickr_toggle_button"));
                                t.setAttribute("data-toggle", ""), e.push(t)
                            }
                            if (!1 !== this.options.flatpickr.showClearButton) {
                                const t = this.getButton("", "clear", this.translate("flatpickr_clear_button"));
                                t.setAttribute("data-clear", ""), e.push(t)
                            }
                            const {parentNode: s} = this.input, {nextSibling: i} = this.input,
                                r = this.theme.getInputGroup(this.input, e);
                            void 0 !== r ? (this.options.flatpickr.inline = !1, s.insertBefore(r, i), t = r) : this.options.flatpickr.wrap = !1
                        }
                        this.flatpickr = window.flatpickr(t, this.options.flatpickr), !0 === this.options.flatpickr.inline && !0 === this.options.flatpickr.inlineHideInput && this.input.setAttribute("type", "hidden")
                    }
                }

                getValue() {
                    if (!this.dependenciesFulfilled) return;
                    if ("string" === this.schema.type) return this.value;
                    if ("" === this.value || void 0 === this.value) return;
                    const t = "time" === this.schema.format ? "1970-01-01 " + this.value : this.value;
                    return parseInt(new Date(t).getTime() / 1e3)
                }

                setValue(t, e, s) {
                    if ("string" === this.schema.type) super.setValue(t, e, s), this.flatpickr && this.flatpickr.setDate(t); else if (t > 0) {
                        const e = new Date(1e3 * t), s = e.getFullYear(), i = this.zeroPad(e.getMonth() + 1),
                            r = this.zeroPad(e.getDate()), o = this.zeroPad(e.getHours()),
                            n = this.zeroPad(e.getMinutes()), a = this.zeroPad(e.getSeconds()), l = [s, i, r].join("-"),
                            h = [o, n, a].join(":");
                        let d = `${l}T${h}`;
                        "date" === this.schema.format ? d = l : "time" === this.schema.format && (d = h), this.input.value = d, this.refreshValue(), this.flatpickr && this.flatpickr.setDate(d)
                    }
                }

                destroy() {
                    this.flatpickr && this.flatpickr.destroy(), this.flatpickr = null, super.destroy()
                }

                zeroPad(t) {
                    return ("0" + t).slice(-2)
                }
            }, describedBy: class extends _ {
                register() {
                    if (this.editors) {
                        for (let t = 0; t < this.editors.length; t++) this.editors[t] && this.editors[t].unregister();
                        this.editors[this.currentEditor] && this.editors[this.currentEditor].register()
                    }
                    super.register()
                }

                unregister() {
                    if (super.unregister(), this.editors) for (let t = 0; t < this.editors.length; t++) this.editors[t] && this.editors[t].unregister()
                }

                getNumColumns() {
                    return this.editors[this.currentEditor] ? Math.max(this.editors[this.currentEditor].getNumColumns(), 4) : 4
                }

                enable() {
                    if (this.editors) for (let t = 0; t < this.editors.length; t++) this.editors[t] && this.editors[t].enable();
                    super.enable()
                }

                disable() {
                    if (this.editors) for (let t = 0; t < this.editors.length; t++) this.editors[t] && this.editors[t].disable();
                    super.disable()
                }

                switchEditor() {
                    const t = this.getWatchedFieldValues();
                    if (!t) return;
                    const e = document.location.origin + document.location.pathname + this.template(t);
                    this.editors[this.refs[e]] || this.buildChildEditor(e), this.currentEditor = this.refs[e], this.register(), this.editors.forEach((t, e) => {
                        t && (this.currentEditor === e ? t.container.style.display = "" : t.container.style.display = "none")
                    }), this.refreshValue(), this.onChange(!0)
                }

                buildChildEditor(t) {
                    this.refs[t] = this.editors.length;
                    const e = this.theme.getChildEditorHolder();
                    this.editor_holder.appendChild(e);
                    const s = c({}, this.schema, this.jsoneditor.refs[t]),
                        i = this.jsoneditor.getEditorClass(s, this.jsoneditor), r = this.jsoneditor.createEditor(i, {
                            jsoneditor: this.jsoneditor,
                            schema: s,
                            container: e,
                            path: this.path,
                            parent: this,
                            required: !0
                        });
                    this.editors.push(r), r.preBuild(), r.build(), r.postBuild()
                }

                preBuild() {
                    let t;
                    for (this.refs = {}, this.editors = [], this.currentEditor = "", t = 0; t < this.schema.links.length; t++) if ("describedby" === this.schema.links[t].rel.toLowerCase()) {
                        this.template = this.jsoneditor.compileTemplate(this.schema.links[t].href, this.template_engine);
                        break
                    }
                    this.schema.links = this.schema.links.slice(0, t).concat(this.schema.links.slice(t + 1)), 0 === this.schema.links.length && delete this.schema.links, this.baseSchema = c({}, this.schema)
                }

                build() {
                    this.editor_holder = document.createElement("div"), this.container.appendChild(this.editor_holder), this.switchEditor()
                }

                onWatchedFieldChange() {
                    this.switchEditor()
                }

                onChildEditorChange(t) {
                    this.editors[this.currentEditor] && this.refreshValue(), super.onChildEditorChange(t)
                }

                refreshValue() {
                    this.editors[this.currentEditor] && (this.value = this.editors[this.currentEditor].getValue())
                }

                setValue(t, e) {
                    this.editors[this.currentEditor] && (this.editors[this.currentEditor].setValue(t, e), this.refreshValue(), this.onChange())
                }

                destroy() {
                    this.editors.forEach(t => {
                        t && t.destroy()
                    }), this.editor_holder && this.editor_holder.parentNode && this.editor_holder.parentNode.removeChild(this.editor_holder), super.destroy()
                }

                showValidationErrors(t) {
                    this.editors.forEach(e => {
                        e && e.showValidationErrors(t)
                    })
                }
            }, enum: class extends _ {
                getNumColumns() {
                    return 4
                }

                build() {
                    this.title = this.header = this.label = this.theme.getFormInputLabel(this.getTitle(), this.isRequired()), this.container.appendChild(this.title), this.options.enum_titles = this.options.enum_titles || [], this.enum = this.schema.enum, this.selected = 0, this.select_options = [], this.html_values = [];
                    for (let t = 0; t < this.enum.length; t++) this.select_options[t] = this.options.enum_titles[t] || "Value " + (t + 1), this.html_values[t] = this.getHTML(this.enum[t]);
                    this.switcher = this.theme.getSwitcher(this.select_options), this.container.appendChild(this.switcher), this.display_area = this.theme.getIndentedPanel(), this.container.appendChild(this.display_area), this.options.hide_display && (this.display_area.style.display = "none"), this.switcher.addEventListener("change", t => {
                        this.selected = this.select_options.indexOf(t.currentTarget.value), this.value = this.enum[this.selected], this.refreshValue(), this.onChange(!0)
                    }), this.value = this.enum[0], this.refreshValue(), 1 === this.enum.length && (this.switcher.style.display = "none")
                }

                refreshValue() {
                    this.selected = -1;
                    const t = JSON.stringify(this.value);
                    this.enum.forEach((e, s) => {
                        if (t === JSON.stringify(e)) return this.selected = s, !1
                    }), this.selected < 0 ? this.setValue(this.enum[0]) : (this.switcher.value = this.select_options[this.selected], this.display_area.innerHTML = this.html_values[this.selected])
                }

                enable() {
                    this.always_disabled || (this.switcher.disabled = !1, super.enable())
                }

                disable(t) {
                    t && (this.always_disabled = !0), this.switcher.disabled = !0, super.disable()
                }

                getHTML(t) {
                    if (null === t) return "<em>null</em>";
                    if ("object" == typeof t) {
                        let e = "";
                        return ((t, e) => {
                            Array.isArray(t) || "number" == typeof t.length && t.length > 0 && t.length - 1 in t ? Array.from(t).forEach((t, s) => e(s, t)) : Object.entries(t).forEach(([t, s]) => e(t, s))
                        })(t, (s, i) => {
                            let r = this.getHTML(i);
                            Array.isArray(t) || (r = `<div><em>${s}</em>: ${r}</div>`), e += `<li>${r}</li>`
                        }), e = Array.isArray(t) ? `<ol>${e}</ol>` : `<ul style='margin-top:0;margin-bottom:0;padding-top:0;padding-bottom:0;'>${e}</ul>`, e
                    }
                    return "boolean" == typeof t ? t ? "true" : "false" : "string" == typeof t ? t.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;") : t
                }

                setValue(t) {
                    this.value !== t && (this.value = t, this.refreshValue(), this.onChange())
                }

                destroy() {
                    this.display_area && this.display_area.parentNode && this.display_area.parentNode.removeChild(this.display_area), this.title && this.title.parentNode && this.title.parentNode.removeChild(this.title), this.switcher && this.switcher.parentNode && this.switcher.parentNode.removeChild(this.switcher), super.destroy()
                }
            }, hidden: class extends _ {
                register() {
                    super.register(), this.input && this.input.setAttribute("name", this.formname)
                }

                unregister() {
                    super.unregister(), this.input && this.input.removeAttribute("name")
                }

                setValue(t, e, s) {
                    if (this.template && !s) return;
                    if (null == t ? t = "" : "object" == typeof t ? t = JSON.stringify(t) : "string" != typeof t && (t = "" + t), t === this.serialized) return;
                    const i = this.sanitize(t);
                    if (this.input.value === i) return;
                    this.input.value = i;
                    const r = s || this.getValue() !== t;
                    this.refreshValue(), e ? this.is_dirty = !1 : "change" === this.jsoneditor.options.show_errors && (this.is_dirty = !0), this.adjust_height && this.adjust_height(this.input), this.onChange(r)
                }

                getNumColumns() {
                    return 2
                }

                enable() {
                    super.enable()
                }

                disable() {
                    super.disable()
                }

                refreshValue() {
                    this.value = this.input.value, "string" != typeof this.value && (this.value = ""), this.serialized = this.value
                }

                destroy() {
                    this.template = null, this.input && this.input.parentNode && this.input.parentNode.removeChild(this.input), this.label && this.label.parentNode && this.label.parentNode.removeChild(this.label), this.description && this.description.parentNode && this.description.parentNode.removeChild(this.description), super.destroy()
                }

                sanitize(t) {
                    return t
                }

                onWatchedFieldChange() {
                    let t;
                    this.template && (t = this.getWatchedFieldValues(), this.setValue(this.template(t), !1, !0)), super.onWatchedFieldChange()
                }

                build() {
                    if (this.format = this.schema.format, !this.format && this.options.default_format && (this.format = this.options.default_format), this.options.format && (this.format = this.options.format), this.input_type = "hidden", this.input = this.theme.getFormInputField(this.input_type), this.format && this.input.setAttribute("data-schemaformat", this.format), this.container.appendChild(this.input), this.schema.template) {
                        const t = this.expandCallbacks("template", {template: this.schema.template});
                        "function" == typeof t.template ? this.template = t.template : this.template = this.jsoneditor.compileTemplate(this.schema.template, this.template_engine), this.refreshValue()
                    } else this.refreshValue()
                }
            }, info: class extends C {
                build() {
                    this.options.compact = !1, this.header = this.label = this.theme.getFormInputLabel(this.getTitle()), this.description = this.theme.getDescription(this.schema.description || ""), this.control = this.theme.getFormControl(this.label, this.description, null), this.container.appendChild(this.control)
                }

                getTitle() {
                    return this.schema.title
                }

                getNumColumns() {
                    return 12
                }
            }, integer: class extends E {
                getNumColumns() {
                    return 2
                }

                getValue() {
                    if (this.dependenciesFulfilled) return function (t) {
                        if (null == t) return !1;
                        const e = t.match(b), s = parseInt(t);
                        return null !== e && !isNaN(s) && isFinite(s)
                    }(this.value) ? parseInt(this.value) : this.value
                }
            }, ip: class extends y {
                preBuild() {
                    if (super.preBuild(), this.schema.options || (this.schema.options = {}), !this.schema.options.cleave) switch (this.format) {
                        case"ipv6":
                            this.schema.options.cleave = {
                                delimiters: [":"],
                                blocks: [4, 4, 4, 4, 4, 4, 4, 4],
                                uppercase: !0
                            };
                            break;
                        case"ipv4":
                            this.schema.options.cleave = {delimiters: ["."], blocks: [3, 3, 3, 3], numericOnly: !0}
                    }
                    this.options = c(this.options, this.schema.options || {})
                }
            }, jodit: class extends y {
                setValue(t, e, s) {
                    const i = super.setValue(t, e, s);
                    void 0 !== i && i.changed && this.jodit_instance && this.jodit_instance.setEditorValue(i.value)
                }

                build() {
                    this.options.format = "textarea", super.build(), this.input_type = this.schema.format, this.input.setAttribute("data-schemaformat", this.input_type)
                }

                afterInputReady() {
                    let t;
                    window.Jodit ? (t = this.expandCallbacks("jodit", c({}, {height: 300}, this.defaults.options.jodit || {}, this.options.jodit || {})), this.jodit_instance = new window.Jodit(this.input, t), (this.schema.readOnly || this.schema.readonly || this.schema.template) && this.jodit_instance.setReadOnly(!0), this.jodit_instance.events.on("change", () => {
                        this.value = this.jodit_instance.getEditorValue(), this.is_dirty = !0, this.onChange(!0)
                    }), this.theme.afterInputReady(this.input)) : super.afterInputReady()
                }

                getNumColumns() {
                    return 6
                }

                enable() {
                    !this.always_disabled && this.jodit_instance && this.jodit_instance.setReadOnly(!1), super.enable()
                }

                disable(t) {
                    this.jodit_instance && this.jodit_instance.setReadOnly(!0), super.disable(t)
                }

                destroy() {
                    this.jodit_instance && (this.jodit_instance.destruct(), this.jodit_instance = null), super.destroy()
                }
            }, multiple: class extends _ {
                register() {
                    if (this.editors) {
                        for (let t = 0; t < this.editors.length; t++) this.editors[t] && this.editors[t].unregister();
                        this.editors[this.type] && this.editors[this.type].register()
                    }
                    super.register()
                }

                unregister() {
                    if (super.unregister(), this.editors) for (let t = 0; t < this.editors.length; t++) this.editors[t] && this.editors[t].unregister()
                }

                getNumColumns() {
                    return this.editors[this.type] ? Math.max(this.editors[this.type].getNumColumns(), 4) : 4
                }

                enable() {
                    if (!this.always_disabled) {
                        if (this.editors) for (let t = 0; t < this.editors.length; t++) this.editors[t] && this.editors[t].enable();
                        this.switcher.disabled = !1, super.enable()
                    }
                }

                disable(t) {
                    if (t && (this.always_disabled = !0), this.editors) for (let e = 0; e < this.editors.length; e++) this.editors[e] && this.editors[e].disable(t);
                    this.switcher.disabled = !0, super.disable()
                }

                switchEditor(t) {
                    this.editors[t] || this.buildChildEditor(t);
                    const e = this.getValue();
                    this.type = t, this.register(), this.editors.forEach((t, s) => {
                        t && (this.type === s ? (this.keep_values && t.setValue(e, !0), t.container.style.display = "") : t.container.style.display = "none")
                    }), this.refreshValue(), this.refreshHeaderText()
                }

                buildChildEditor(t) {
                    const e = this.types[t], s = this.theme.getChildEditorHolder();
                    let i;
                    this.editor_holder.appendChild(s), "string" == typeof e ? (i = c({}, this.schema), i.type = e) : (i = c({}, this.schema, e), i = this.jsoneditor.expandRefs(i), e && e.required && Array.isArray(e.required) && this.schema.required && Array.isArray(this.schema.required) && (i.required = this.schema.required.concat(e.required)));
                    const r = this.jsoneditor.getEditorClass(i);
                    this.editors[t] = this.jsoneditor.createEditor(r, {
                        jsoneditor: this.jsoneditor,
                        schema: i,
                        container: s,
                        path: this.path,
                        parent: this,
                        required: !0
                    }), this.editors[t].preBuild(), this.editors[t].build(), this.editors[t].postBuild(), this.editors[t].header && (this.editors[t].header.style.display = "none"), this.editors[t].option = this.switcher_options[t], s.addEventListener("change_header_text", () => {
                        this.refreshHeaderText()
                    }), t !== this.type && (s.style.display = "none")
                }

                preBuild() {
                    if (this.types = [], this.type = 0, this.editors = [], this.validators = [], this.keep_values = !0, void 0 !== this.jsoneditor.options.keep_oneof_values && (this.keep_values = this.jsoneditor.options.keep_oneof_values), void 0 !== this.options.keep_oneof_values && (this.keep_values = this.options.keep_oneof_values), this.schema.oneOf) this.oneOf = !0, this.types = this.schema.oneOf, delete this.schema.oneOf; else if (this.schema.anyOf) this.anyOf = !0, this.types = this.schema.anyOf, delete this.schema.anyOf; else {
                        if (this.schema.type && "any" !== this.schema.type) Array.isArray(this.schema.type) ? this.types = this.schema.type : this.types = [this.schema.type]; else if (this.types = ["string", "number", "integer", "boolean", "object", "array", "null"], this.schema.disallow) {
                            let {disallow: t} = this.schema;
                            "object" == typeof t && Array.isArray(t) || (t = [t]);
                            const e = [];
                            this.types.forEach(s => {
                                t.includes(s) || e.push(s)
                            }), this.types = e
                        }
                        delete this.schema.type
                    }
                    this.display_text = this.getDisplayText(this.types)
                }

                build() {
                    const {container: t} = this;
                    this.header = this.label = this.theme.getFormInputLabel(this.getTitle(), this.isRequired()), this.container.appendChild(this.header), this.switcher = this.theme.getSwitcher(this.display_text), t.appendChild(this.switcher), this.switcher.addEventListener("change", t => {
                        t.preventDefault(), t.stopPropagation(), this.switchEditor(this.display_text.indexOf(t.currentTarget.value)), this.onChange(!0)
                    }), this.editor_holder = document.createElement("div"), t.appendChild(this.editor_holder);
                    const e = {};
                    this.jsoneditor.options.custom_validators && (e.custom_validators = this.jsoneditor.options.custom_validators), this.switcher_options = this.theme.getSwitcherOptions(this.switcher), this.types.forEach((t, s) => {
                        let i;
                        this.editors[s] = !1, "string" == typeof t ? (i = c({}, this.schema), i.type = t) : (i = c({}, this.schema, t), t.required && Array.isArray(t.required) && this.schema.required && Array.isArray(this.schema.required) && (i.required = this.schema.required.concat(t.required))), this.validators[s] = new g(this.jsoneditor, i, e, this.defaults)
                    }), this.switchEditor(0)
                }

                onChildEditorChange(t) {
                    this.editors[this.type] && (this.refreshValue(), this.refreshHeaderText()), super.onChildEditorChange()
                }

                refreshHeaderText() {
                    const t = this.getDisplayText(this.types);
                    Array.from(this.switcher_options).forEach((e, s) => {
                        e.textContent = t[s]
                    })
                }

                refreshValue() {
                    this.value = this.editors[this.type].getValue()
                }

                setValue(t, e) {
                    const s = this.type;
                    let i = {match: 0, extra: 0, i: this.type};
                    const r = {match: 0, i: null};
                    this.validators.forEach((e, s) => {
                        let o = null;
                        void 0 !== this.anyOf && this.anyOf && (o = e.fitTest(t), (i.match < o.match || i.match === o.match && i.extra > o.extra) && (i = o, i.i = s)), e.validate(t).length || null !== r.i || (r.i = s, null !== o && (r.match = o.match))
                    });
                    let o = r.i;
                    void 0 !== this.anyOf && this.anyOf && r.match < i.match && (o = i.i), null === o && (o = this.type), this.type = o, this.switcher.value = this.display_text[o];
                    const n = this.type !== s;
                    n && this.switchEditor(this.type), this.editors[this.type].setValue(t, e), this.refreshValue(), this.onChange(n)
                }

                destroy() {
                    this.editors.forEach(t => {
                        t && t.destroy()
                    }), this.editor_holder && this.editor_holder.parentNode && this.editor_holder.parentNode.removeChild(this.editor_holder), this.switcher && this.switcher.parentNode && this.switcher.parentNode.removeChild(this.switcher), super.destroy()
                }

                showValidationErrors(t) {
                    if (this.oneOf || this.anyOf) {
                        const e = this.oneOf ? "oneOf" : "anyOf";
                        this.editors.forEach((s, i) => {
                            if (!s) return;
                            const r = `${this.path}.${e}[${i}]`;
                            s.showValidationErrors(t.reduce((t, e) => {
                                if (e.path === r.substr(0, e.path.length)) {
                                    const s = c({}, e);
                                    s.path = this.path + s.path.substr(r.length), t.push(s)
                                }
                                return t
                            }, []))
                        })
                    } else this.editors.forEach(e => {
                        e && e.showValidationErrors(t)
                    })
                }

                addLinks() {
                }
            }, multiselect: v, null: class extends _ {
                getValue() {
                    if (this.dependenciesFulfilled) return null
                }

                setValue() {
                    this.onChange()
                }

                getNumColumns() {
                    return 2
                }
            }, number: E, object: class extends _ {
                constructor(t, e, s) {
                    super(t, e), this.currentDepth = s
                }

                getDefault() {
                    return c({}, this.schema.default || {})
                }

                getChildEditors() {
                    return this.editors
                }

                register() {
                    super.register(), this.editors && Object.values(this.editors).forEach(t => t.register())
                }

                unregister() {
                    super.unregister(), this.editors && Object.values(this.editors).forEach(t => t.unregister())
                }

                getNumColumns() {
                    return Math.max(Math.min(12, this.maxwidth), 3)
                }

                enable() {
                    this.always_disabled || (this.editjson_control && (this.editjson_control.disabled = !1), this.addproperty_button && (this.addproperty_button.disabled = !1), super.enable(), this.editors && Object.values(this.editors).forEach(t => {
                        t.isActive() && t.enable(), t.optInCheckbox.disabled = !1
                    }))
                }

                disable(t) {
                    t && (this.always_disabled = !0), this.editjson_control && (this.editjson_control.disabled = !0), this.addproperty_button && (this.addproperty_button.disabled = !0), this.hideEditJSON(), super.disable(), this.editors && Object.values(this.editors).forEach(e => {
                        e.isActive() && e.disable(t), e.optInCheckbox.disabled = !0
                    })
                }

                layoutEditors() {
                    let t, e, s;
                    if (!this.row_container) return;
                    this.property_order = Object.keys(this.editors), this.property_order = this.property_order.sort((t, e) => {
                        let s = this.editors[t].schema.propertyOrder, i = this.editors[e].schema.propertyOrder;
                        return "number" != typeof s && (s = 1e3), "number" != typeof i && (i = 1e3), s - i
                    });
                    const i = "categories" === this.format, r = [];
                    let o, n = null, a = null;
                    if ("grid-strict" === this.format) {
                        let i = 0;
                        if (o = [], this.property_order.forEach(t => {
                            const e = this.editors[t];
                            if (e.property_removed) return;
                            const s = e.options.hidden ? 0 : e.options.grid_columns || e.getNumColumns(),
                                n = e.options.hidden ? 0 : e.options.grid_offset || 0,
                                a = !e.options.hidden && (e.options.grid_break || !1), l = {
                                    key: t,
                                    width: s,
                                    offset: n,
                                    height: e.options.hidden ? 0 : e.container.offsetHeight
                                };
                            o.push(l), r[i] = o, a && (i++, o = [])
                        }), this.layout === JSON.stringify(r)) return !1;
                        for (this.layout = JSON.stringify(r), s = document.createElement("div"), t = 0; t < r.length; t++) for (o = this.theme.getGridRow(), s.appendChild(o), e = 0; e < r[t].length; e++) n = r[t][e].key, a = this.editors[n], a.options.hidden ? a.container.style.display = "none" : this.theme.setGridColumnSize(a.container, r[t][e].width, r[t][e].offset), o.appendChild(a.container)
                    } else if ("grid" === this.format) {
                        for (this.property_order.forEach(t => {
                            const e = this.editors[t];
                            if (e.property_removed) return;
                            let s = !1;
                            const i = e.options.hidden ? 0 : e.options.grid_columns || e.getNumColumns(),
                                o = e.options.hidden ? 0 : e.container.offsetHeight;
                            for (let t = 0; t < r.length; t++) r[t].width + i <= 12 && (!o || .5 * r[t].minh < o && 2 * r[t].maxh > o) && (s = t);
                            !1 === s && (r.push({
                                width: 0,
                                minh: 999999,
                                maxh: 0,
                                editors: []
                            }), s = r.length - 1), r[s].editors.push({
                                key: t,
                                width: i,
                                height: o
                            }), r[s].width += i, r[s].minh = Math.min(r[s].minh, o), r[s].maxh = Math.max(r[s].maxh, o)
                        }), t = 0; t < r.length; t++) if (r[t].width < 12) {
                            let s = !1, i = 0;
                            for (e = 0; e < r[t].editors.length; e++) (!1 === s || r[t].editors[e].width > r[t].editors[s].width) && (s = e), r[t].editors[e].width *= 12 / r[t].width, r[t].editors[e].width = Math.floor(r[t].editors[e].width), i += r[t].editors[e].width;
                            i < 12 && (r[t].editors[s].width += 12 - i), r[t].width = 12
                        }
                        if (this.layout === JSON.stringify(r)) return !1;
                        for (this.layout = JSON.stringify(r), s = document.createElement("div"), t = 0; t < r.length; t++) for (o = this.theme.getGridRow(), s.appendChild(o), e = 0; e < r[t].editors.length; e++) n = r[t].editors[e].key, a = this.editors[n], a.options.hidden ? a.container.style.display = "none" : this.theme.setGridColumnSize(a.container, r[t].editors[e].width), o.appendChild(a.container)
                    } else {
                        if (s = document.createElement("div"), i) {
                            const t = document.createElement("div"), e = this.theme.getTopTabHolder(this.schema.title),
                                s = this.theme.getTopTabContentHolder(e);
                            for (this.property_order.forEach(i => {
                                const r = this.editors[i];
                                if (r.property_removed) return;
                                const o = this.theme.getTabContent(),
                                    n = r.schema && ("object" === r.schema.type || "array" === r.schema.type);
                                o.isObjOrArray = n;
                                const a = this.theme.getGridRow();
                                r.tab || (void 0 === this.basicPane ? this.addRow(r, e, o) : this.addRow(r, e, this.basicPane)), o.id = this.getValidId(r.tab_text.textContent), n ? (o.appendChild(a), s.appendChild(o), this.theme.addTopTab(e, r.tab)) : (t.appendChild(a), s.childElementCount > 0 ? s.firstChild.isObjOrArray && (o.appendChild(t), s.insertBefore(o, s.firstChild), this.theme.insertBasicTopTab(r.tab, e), r.basicPane = o) : (o.appendChild(t), s.appendChild(o), this.theme.addTopTab(e, r.tab), r.basicPane = o)), r.options.hidden ? r.container.style.display = "none" : this.theme.setGridColumnSize(r.container, 12), a.appendChild(r.container), r.rowPane = o
                            }); this.tabPanesContainer.firstChild;) this.tabPanesContainer.removeChild(this.tabPanesContainer.firstChild);
                            const i = this.tabs_holder.parentNode;
                            i.removeChild(i.firstChild), i.appendChild(e), this.tabPanesContainer = s, this.tabs_holder = e;
                            const r = this.theme.getFirstTab(this.tabs_holder);
                            return void (r && p(r, "click"))
                        }
                        this.property_order.forEach(t => {
                            const e = this.editors[t];
                            e.property_removed || (o = this.theme.getGridRow(), s.appendChild(o), e.options.hidden ? e.container.style.display = "none" : this.theme.setGridColumnSize(e.container, 12), o.appendChild(e.container))
                        })
                    }
                    for (; this.row_container.firstChild;) this.row_container.removeChild(this.row_container.firstChild);
                    this.row_container.appendChild(s)
                }

                getPropertySchema(t) {
                    let e = this.schema.properties[t] || {};
                    e = c({}, e);
                    let s = !!this.schema.properties[t];
                    return this.schema.patternProperties && Object.keys(this.schema.patternProperties).forEach(i => {
                        new RegExp(i).test(t) && (e.allOf = e.allOf || [], e.allOf.push(this.schema.patternProperties[i]), s = !0)
                    }), !s && this.schema.additionalProperties && "object" == typeof this.schema.additionalProperties && (e = c({}, this.schema.additionalProperties)), e
                }

                preBuild() {
                    if (super.preBuild(), this.editors = {}, this.cached_editors = {}, this.format = this.options.layout || this.options.object_layout || this.schema.format || this.jsoneditor.options.object_layout || "normal", this.schema.properties = this.schema.properties || {}, this.minwidth = 0, this.maxwidth = 0, this.options.table_row) Object.entries(this.schema.properties).forEach(([t, e]) => {
                        const s = this.jsoneditor.getEditorClass(e);
                        this.editors[t] = this.jsoneditor.createEditor(s, {
                            jsoneditor: this.jsoneditor,
                            schema: e,
                            path: `${this.path}.${t}`,
                            parent: this,
                            compact: !0,
                            required: !0
                        }, this.currentDepth + 1), this.editors[t].preBuild();
                        const i = this.editors[t].options.hidden ? 0 : this.editors[t].options.grid_columns || this.editors[t].getNumColumns();
                        this.minwidth += i, this.maxwidth += i
                    }), this.no_link_holder = !0; else {
                        if (this.options.table) throw new Error("Not supported yet");
                        this.schema.defaultProperties || (this.jsoneditor.options.display_required_only || this.options.display_required_only ? this.schema.defaultProperties = Object.keys(this.schema.properties).filter(t => this.isRequiredObject({
                            key: t,
                            schema: this.schema.properties[t]
                        })) : this.schema.defaultProperties = Object.keys(this.schema.properties)), this.maxwidth += 1, this.schema.defaultProperties.forEach(t => {
                            this.addObjectProperty(t, !0), this.editors[t] && (this.minwidth = Math.max(this.minwidth, this.editors[t].options.grid_columns || this.editors[t].getNumColumns()), this.maxwidth += this.editors[t].options.grid_columns || this.editors[t].getNumColumns())
                        })
                    }
                    this.property_order = Object.keys(this.editors), this.property_order = this.property_order.sort((t, e) => {
                        let s = this.editors[t].schema.propertyOrder, i = this.editors[e].schema.propertyOrder;
                        return "number" != typeof s && (s = 1e3), "number" != typeof i && (i = 1e3), s - i
                    })
                }

                addTab(t) {
                    const e = this.rows[t].schema && ("object" === this.rows[t].schema.type || "array" === this.rows[t].schema.type);
                    this.tabs_holder && (this.rows[t].tab_text = document.createElement("span"), this.rows[t].tab_text.textContent = e ? this.rows[t].getHeaderText() : void 0 === this.schema.basicCategoryTitle ? "Basic" : this.schema.basicCategoryTitle, this.rows[t].tab = this.theme.getTopTab(this.rows[t].tab_text, this.getValidId(this.rows[t].tab_text.textContent)), this.rows[t].tab.addEventListener("click", e => {
                        this.active_tab = this.rows[t].tab, this.refreshTabs(), e.preventDefault(), e.stopPropagation()
                    }))
                }

                addRow(t, e, s) {
                    const i = this.rows.length, r = "object" === t.schema.type || "array" === t.schema.type;
                    this.rows[i] = t, this.rows[i].rowPane = s, r ? (this.addTab(i), this.theme.addTopTab(e, this.rows[i].tab)) : void 0 === this.basicTab ? (this.addTab(i), this.basicTab = i, this.basicPane = s, this.theme.addTopTab(e, this.rows[i].tab)) : (this.rows[i].tab = this.rows[this.basicTab].tab, this.rows[i].tab_text = this.rows[this.basicTab].tab_text, this.rows[i].rowPane = this.rows[this.basicTab].rowPane)
                }

                refreshTabs(t) {
                    const e = void 0 !== this.basicTab;
                    let s = !1;
                    this.rows.forEach(i => {
                        i.tab && i.rowPane && i.rowPane.parentNode && (e && i.tab === this.rows[this.basicTab].tab && s || (t ? i.tab_text.textContent = i.getHeaderText() : (e && i.tab === this.rows[this.basicTab].tab && (s = !0), i.tab === this.active_tab ? this.theme.markTabActive(i) : this.theme.markTabInactive(i))))
                    })
                }

                build() {
                    const t = "categories" === this.format;
                    if (this.rows = [], this.active_tab = null, this.options.table_row) this.editor_holder = this.container, Object.entries(this.editors).forEach(([t, e]) => {
                        const s = this.theme.getTableCell();
                        this.editor_holder.appendChild(s), e.setContainer(s), e.build(), e.postBuild(), e.setOptInCheckbox(e.header), this.editors[t].options.hidden && (s.style.display = "none"), this.editors[t].options.input_width && (s.style.width = this.editors[t].options.input_width)
                    }); else {
                        if (this.options.table) throw new Error("Not supported yet");
                        {
                            this.header = "", this.options.compact || (this.header = document.createElement("label"), this.header.textContent = this.getTitle()), this.title = this.theme.getHeader(this.header), this.controls = this.theme.getButtonHolder(), this.controls.style.margin = "0 0 0 10px", this.container.appendChild(this.title), this.container.appendChild(this.controls), this.container.style.position = "relative", this.editjson_holder = this.theme.getModal(), this.editjson_textarea = this.theme.getTextareaInput(), this.editjson_textarea.style.height = "170px", this.editjson_textarea.style.width = "300px", this.editjson_textarea.style.display = "block", this.editjson_save = this.getButton("Save", "save", "Save"), this.editjson_save.classList.add("json-editor-btntype-save"), this.editjson_save.addEventListener("click", t => {
                                t.preventDefault(), t.stopPropagation(), this.saveJSON()
                            }), this.editjson_copy = this.getButton("Copy", "copy", "Copy"), this.editjson_copy.classList.add("json-editor-btntype-copy"), this.editjson_copy.addEventListener("click", t => {
                                t.preventDefault(), t.stopPropagation(), this.copyJSON()
                            }), this.editjson_cancel = this.getButton("Cancel", "cancel", "Cancel"), this.editjson_cancel.classList.add("json-editor-btntype-cancel"), this.editjson_cancel.addEventListener("click", t => {
                                t.preventDefault(), t.stopPropagation(), this.hideEditJSON()
                            }), this.editjson_holder.appendChild(this.editjson_textarea), this.editjson_holder.appendChild(this.editjson_save), this.editjson_holder.appendChild(this.editjson_copy), this.editjson_holder.appendChild(this.editjson_cancel), this.addproperty_holder = this.theme.getModal(), this.addproperty_list = document.createElement("div"), this.addproperty_list.style.width = "295px", this.addproperty_list.style.maxHeight = "160px", this.addproperty_list.style.padding = "5px 0", this.addproperty_list.style.overflowY = "auto", this.addproperty_list.style.overflowX = "hidden", this.addproperty_list.style.paddingLeft = "5px", this.addproperty_list.setAttribute("class", "property-selector"), this.addproperty_add = this.getButton("add", "add", "add"), this.addproperty_add.classList.add("json-editor-btntype-add"), this.addproperty_input = this.theme.getFormInputField("text"), this.addproperty_input.setAttribute("placeholder", "Property name..."), this.addproperty_input.style.width = "220px", this.addproperty_input.style.marginBottom = "0", this.addproperty_input.style.display = "inline-block", this.addproperty_add.addEventListener("click", t => {
                                if (t.preventDefault(), t.stopPropagation(), this.addproperty_input.value) {
                                    if (this.editors[this.addproperty_input.value]) return void window.alert("there is already a property with that name");
                                    this.addObjectProperty(this.addproperty_input.value), this.editors[this.addproperty_input.value] && this.editors[this.addproperty_input.value].disable(), this.onChange(!0)
                                }
                            }), this.addproperty_input.addEventListener("input", t => {
                                t.target.previousSibling.childNodes.forEach(e => {
                                    e.innerText.includes(t.target.value) ? e.style.display = "" : e.style.display = "none"
                                })
                            }), this.addproperty_holder.appendChild(this.addproperty_list), this.addproperty_holder.appendChild(this.addproperty_input), this.addproperty_holder.appendChild(this.addproperty_add);
                            const e = document.createElement("div");
                            e.style.clear = "both", this.addproperty_holder.appendChild(e), document.addEventListener("click", this.onOutsideModalClick), this.schema.description && (this.description = this.theme.getDescription(this.schema.description), this.container.appendChild(this.description)), this.error_holder = document.createElement("div"), this.container.appendChild(this.error_holder), this.editor_holder = this.theme.getIndentedPanel(), this.container.appendChild(this.editor_holder), this.row_container = this.theme.getGridContainer(), t ? (this.tabs_holder = this.theme.getTopTabHolder(this.getValidId(this.schema.title)), this.tabPanesContainer = this.theme.getTopTabContentHolder(this.tabs_holder), this.editor_holder.appendChild(this.tabs_holder)) : (this.tabs_holder = this.theme.getTabHolder(this.getValidId(this.schema.title)), this.tabPanesContainer = this.theme.getTabContentHolder(this.tabs_holder), this.editor_holder.appendChild(this.row_container)), Object.values(this.editors).forEach(e => {
                                const s = this.theme.getTabContent(), i = this.theme.getGridColumn(),
                                    r = !(!e.schema || "object" !== e.schema.type && "array" !== e.schema.type);
                                if (s.isObjOrArray = r, t) {
                                    if (r) {
                                        const t = this.theme.getGridContainer();
                                        t.appendChild(i), s.appendChild(t), this.tabPanesContainer.appendChild(s), this.row_container = t
                                    } else void 0 === this.row_container_basic && (this.row_container_basic = this.theme.getGridContainer(), s.appendChild(this.row_container_basic), 0 === this.tabPanesContainer.childElementCount ? this.tabPanesContainer.appendChild(s) : this.tabPanesContainer.insertBefore(s, this.tabPanesContainer.childNodes[1])), this.row_container_basic.appendChild(i);
                                    this.addRow(e, this.tabs_holder, s), s.id = this.getValidId(e.schema.title)
                                } else this.row_container.appendChild(i);
                                e.setContainer(i), e.build(), e.postBuild(), e.setOptInCheckbox(e.header)
                            }), this.rows[0] && p(this.rows[0].tab, "click"), this.collapsed = !1, this.collapse_control = this.getButton("", "collapse", this.translate("button_collapse")), this.collapse_control.style.margin = "0 10px 0 0", this.collapse_control.classList.add("json-editor-btntype-toggle"), this.title.insertBefore(this.collapse_control, this.title.childNodes[0]), this.collapse_control.addEventListener("click", t => {
                                t.preventDefault(), t.stopPropagation(), this.collapsed ? (this.editor_holder.style.display = "", this.collapsed = !1, this.setButtonText(this.collapse_control, "", "collapse", this.translate("button_collapse"))) : (this.editor_holder.style.display = "none", this.collapsed = !0, this.setButtonText(this.collapse_control, "", "expand", this.translate("button_expand")))
                            }), this.options.collapsed && p(this.collapse_control, "click"), this.schema.options && void 0 !== this.schema.options.disable_collapse ? this.schema.options.disable_collapse && (this.collapse_control.style.display = "none") : this.jsoneditor.options.disable_collapse && (this.collapse_control.style.display = "none"), this.editjson_control = this.getButton("JSON", "edit", "Edit JSON"), this.editjson_control.classList.add("json-editor-btntype-editjson"), this.editjson_control.addEventListener("click", t => {
                                t.preventDefault(), t.stopPropagation(), this.toggleEditJSON()
                            }), this.controls.appendChild(this.editjson_control), this.controls.insertBefore(this.editjson_holder, this.controls.childNodes[0]), this.schema.options && void 0 !== this.schema.options.disable_edit_json ? this.schema.options.disable_edit_json && (this.editjson_control.style.display = "none") : this.jsoneditor.options.disable_edit_json && (this.editjson_control.style.display = "none"), this.addproperty_button = this.getButton("Properties", "edit_properties", this.translate("button_object_properties")), this.addproperty_button.classList.add("json-editor-btntype-properties"), this.addproperty_button.addEventListener("click", t => {
                                t.preventDefault(), t.stopPropagation(), this.toggleAddProperty()
                            }), this.controls.appendChild(this.addproperty_button), this.controls.insertBefore(this.addproperty_holder, this.controls.childNodes[1]), this.refreshAddProperties(), this.deactivateNonRequiredProperties()
                        }
                    }
                    this.options.table_row ? (this.editor_holder = this.container, this.property_order.forEach(t => {
                        this.editor_holder.appendChild(this.editors[t].container)
                    })) : (this.layoutEditors(), this.layoutEditors())
                }

                deactivateNonRequiredProperties() {
                    (this.jsoneditor.options.show_opt_in || this.options.show_opt_in) && Object.entries(this.editors).forEach(([t, e]) => {
                        this.isRequiredObject(e) || this.editors[t].deactivate()
                    })
                }

                showEditJSON() {
                    this.editjson_holder && (this.hideAddProperty(), this.editjson_holder.style.left = this.editjson_control.offsetLeft + "px", this.editjson_holder.style.top = this.editjson_control.offsetTop + this.editjson_control.offsetHeight + "px", this.editjson_textarea.value = JSON.stringify(this.getValue(), null, 2), this.disable(), this.editjson_holder.style.display = "", this.editjson_control.disabled = !1, this.editing_json = !0)
                }

                hideEditJSON() {
                    this.editjson_holder && this.editing_json && (this.editjson_holder.style.display = "none", this.enable(), this.editing_json = !1)
                }

                copyJSON() {
                    if (!this.editjson_holder) return;
                    const t = document.createElement("textarea");
                    t.value = this.editjson_textarea.value, t.setAttribute("readonly", ""), t.style.position = "absolute", t.style.left = "-9999px", document.body.appendChild(t), t.select(), document.execCommand("copy"), document.body.removeChild(t)
                }

                saveJSON() {
                    if (this.editjson_holder) try {
                        const t = JSON.parse(this.editjson_textarea.value);
                        this.setValue(t), this.hideEditJSON(), this.onChange(!0)
                    } catch (t) {
                        throw window.alert("invalid JSON"), t
                    }
                }

                toggleEditJSON() {
                    this.editing_json ? this.hideEditJSON() : this.showEditJSON()
                }

                insertPropertyControlUsingPropertyOrder(t, e, s) {
                    let i;
                    this.schema.properties[t] && (i = this.schema.properties[t].propertyOrder), "number" != typeof i && (i = 1e3), e.propertyOrder = i;
                    for (let t = 0; t < s.childNodes.length; t++) {
                        const i = s.childNodes[t];
                        if (e.propertyOrder < i.propertyOrder) {
                            this.addproperty_list.insertBefore(e, i), e = null;
                            break
                        }
                    }
                    e && this.addproperty_list.appendChild(e)
                }

                addPropertyCheckbox(t) {
                    let e;
                    const s = this.theme.getCheckbox();
                    s.style.width = "auto", e = this.schema.properties[t] && this.schema.properties[t].title ? this.schema.properties[t].title : t;
                    const i = this.theme.getCheckboxLabel(e), r = this.theme.getFormControl(i, s);
                    return r.style.paddingBottom = r.style.marginBottom = r.style.paddingTop = r.style.marginTop = 0, r.style.height = "auto", this.insertPropertyControlUsingPropertyOrder(t, r, this.addproperty_list), s.checked = t in this.editors, s.addEventListener("change", () => {
                        s.checked ? this.addObjectProperty(t) : this.removeObjectProperty(t), this.onChange(!0)
                    }), this.addproperty_checkboxes[t] = s, s
                }

                showAddProperty() {
                    this.addproperty_holder && (this.hideEditJSON(), this.addproperty_holder.style.left = this.addproperty_button.offsetLeft + "px", this.addproperty_holder.style.top = this.addproperty_button.offsetTop + this.addproperty_button.offsetHeight + "px", this.disable(), this.adding_property = !0, this.addproperty_button.disabled = !1, this.addproperty_holder.style.display = "", this.refreshAddProperties())
                }

                hideAddProperty() {
                    this.addproperty_holder && this.adding_property && (this.addproperty_holder.style.display = "none", this.enable(), this.adding_property = !1)
                }

                toggleAddProperty() {
                    this.adding_property ? this.hideAddProperty() : this.showAddProperty()
                }

                removeObjectProperty(t) {
                    this.editors[t] && (this.editors[t].unregister(), delete this.editors[t], this.refreshValue(), this.layoutEditors())
                }

                getSchemaOnMaxDepth(t) {
                    return Object.keys(t).reduce((e, s) => {
                        switch (s) {
                            case"$ref":
                                return e;
                            case"properties":
                            case"items":
                                return {...e, [s]: {}};
                            case"additionalProperties":
                                return {...e, [s]: !0};
                            default:
                                return {...e, [s]: t[s]}
                        }
                    }, {})
                }

                addObjectProperty(t, e) {
                    if (!this.editors[t]) {
                        if (this.cached_editors[t]) {
                            if (this.editors[t] = this.cached_editors[t], e) return;
                            this.editors[t].register()
                        } else {
                            if (!(this.canHaveAdditionalProperties() || this.schema.properties && this.schema.properties[t])) return;
                            const s = this.getPropertySchema(t);
                            "number" != typeof s.propertyOrder && (s.propertyOrder = Object.keys(this.editors).length + 1e3);
                            const i = this.jsoneditor.getEditorClass(s), {max_depth: r} = this.jsoneditor.options;
                            if (this.editors[t] = this.jsoneditor.createEditor(i, {
                                jsoneditor: this.jsoneditor,
                                schema: r && this.currentDepth >= r ? this.getSchemaOnMaxDepth(s) : s,
                                path: `${this.path}.${t}`,
                                parent: this
                            }, this.currentDepth + 1), this.editors[t].preBuild(), !e) {
                                const e = this.theme.getChildEditorHolder();
                                this.editor_holder.appendChild(e), this.editors[t].setContainer(e), this.editors[t].build(), this.editors[t].postBuild(), this.editors[t].setOptInCheckbox(i.header), this.editors[t].activate()
                            }
                            this.cached_editors[t] = this.editors[t]
                        }
                        e || (this.refreshValue(), this.layoutEditors())
                    }
                }

                onOutsideModalClick(t) {
                    this.addproperty_holder && !this.addproperty_holder.contains(t.path[0] || t.composedPath()[0]) && this.adding_property && (t.preventDefault(), t.stopPropagation(), this.toggleAddProperty())
                }

                onChildEditorChange(t) {
                    this.refreshValue(), super.onChildEditorChange(t)
                }

                canHaveAdditionalProperties() {
                    return "boolean" == typeof this.schema.additionalProperties ? this.schema.additionalProperties : !this.jsoneditor.options.no_additional_properties
                }

                destroy() {
                    Object.values(this.cached_editors).forEach(t => t.destroy()), this.editor_holder && (this.editor_holder.innerHTML = ""), this.title && this.title.parentNode && this.title.parentNode.removeChild(this.title), this.error_holder && this.error_holder.parentNode && this.error_holder.parentNode.removeChild(this.error_holder), this.editors = null, this.cached_editors = null, this.editor_holder && this.editor_holder.parentNode && this.editor_holder.parentNode.removeChild(this.editor_holder), this.editor_holder = null, document.removeEventListener("click", this.onOutsideModalClick), super.destroy()
                }

                getValue() {
                    if (!this.dependenciesFulfilled) return;
                    const t = super.getValue();
                    return t && (this.jsoneditor.options.remove_empty_properties || this.options.remove_empty_properties) && Object.keys(t).forEach(e => {
                        var s;
                        (void 0 === (s = t[e]) || "" === s || s === Object(s) && 0 === Object.keys(s).length && s.constructor === Object) && delete t[e]
                    }), t
                }

                refreshValue() {
                    this.value = {}, Object.keys(this.editors).forEach(t => {
                        this.editors[t].isActive() && (this.value[t] = this.editors[t].getValue())
                    }), this.adding_property && this.refreshAddProperties()
                }

                refreshAddProperties() {
                    if (this.options.disable_properties || !1 !== this.options.disable_properties && this.jsoneditor.options.disable_properties) return void (this.addproperty_button.style.display = "none");
                    let t = !1, e = 0, s = !1;
                    Object.keys(this.editors).forEach(t => e++), t = this.canHaveAdditionalProperties() && !(void 0 !== this.schema.maxProperties && e >= this.schema.maxProperties), this.addproperty_checkboxes && (this.addproperty_list.innerHTML = ""), this.addproperty_checkboxes = {}, Object.keys(this.cached_editors).forEach(i => {
                        this.addPropertyCheckbox(i), this.isRequiredObject(this.cached_editors[i]) && i in this.editors && (this.addproperty_checkboxes[i].disabled = !0), void 0 !== this.schema.minProperties && e <= this.schema.minProperties ? (this.addproperty_checkboxes[i].disabled = this.addproperty_checkboxes[i].checked, this.addproperty_checkboxes[i].checked || (s = !0)) : i in this.editors ? s = !0 : t || u(this.schema.properties, i) ? (this.addproperty_checkboxes[i].disabled = !1, s = !0) : this.addproperty_checkboxes[i].disabled = !0
                    }), this.canHaveAdditionalProperties() && (s = !0), Object.keys(this.schema.properties).forEach(t => {
                        this.cached_editors[t] || (s = !0, this.addPropertyCheckbox(t))
                    }), s ? this.canHaveAdditionalProperties() ? this.addproperty_add.disabled = !t : (this.addproperty_add.style.display = "none", this.addproperty_input.style.display = "none") : (this.hideAddProperty(), this.addproperty_button.style.display = "none")
                }

                isRequiredObject(t) {
                    if (t) return "boolean" == typeof t.schema.required ? t.schema.required : Array.isArray(this.schema.required) ? this.schema.required.includes(t.key) : !!this.jsoneditor.options.required_by_default
                }

                setValue(t, e) {
                    ("object" != typeof (t = t || {}) || Array.isArray(t)) && (t = {}), Object.entries(this.cached_editors).forEach(([s, i]) => {
                        void 0 !== t[s] ? (this.addObjectProperty(s), i.setValue(t[s], e)) : e || this.isRequiredObject(i) ? i.setValue(i.getDefault(), e) : this.removeObjectProperty(s)
                    }), Object.entries(t).forEach(([t, s]) => {
                        this.cached_editors[t] || (this.addObjectProperty(t), this.editors[t] && this.editors[t].setValue(s, e))
                    }), this.refreshValue(), this.layoutEditors(), this.onChange()
                }

                showValidationErrors(t) {
                    const e = [], s = [];
                    t.forEach(t => {
                        t.path === this.path ? e.push(t) : s.push(t)
                    }), this.error_holder && (e.length ? (this.error_holder.innerHTML = "", this.error_holder.style.display = "", e.forEach(t => {
                        t.errorcount && t.errorcount > 1 && (t.message += ` (${t.errorcount} errors)`), this.error_holder.appendChild(this.theme.getErrorMessage(t.message))
                    })) : this.error_holder.style.display = "none"), this.options.table_row && (e.length ? this.theme.addTableRowError(this.container) : this.theme.removeTableRowError(this.container)), Object.values(this.editors).forEach(t => {
                        t.showValidationErrors(s)
                    })
                }
            }, radio: class extends x {
                preBuild() {
                    this.schema.required = !0, super.preBuild()
                }

                build() {
                    this.label = "", this.options.compact || (this.header = this.label = this.theme.getFormInputLabel(this.getTitle(), this.isRequired())), this.schema.description && (this.description = this.theme.getFormInputDescription(this.schema.description)), this.options.infoText && (this.infoButton = this.theme.getInfoButton(this.options.infoText)), this.options.compact && this.container.classList.add("compact"), this.radioContainer = document.createElement("div"), this.radioGroup = [];
                    const t = t => {
                        this.setValue(t.currentTarget.value), this.onChange(!0)
                    };
                    for (let e = 0; e < this.enum_values.length; e++) {
                        this.input = this.theme.getFormRadio({
                            name: this.formname,
                            id: `${this.formname}[${e}]`,
                            value: this.enum_values[e]
                        }), this.setInputAttributes(["id", "value", "name"]), this.input.addEventListener("change", t, !1), this.radioGroup.push(this.input);
                        const s = this.theme.getFormRadioLabel(this.enum_display[e]);
                        s.htmlFor = this.input.id;
                        const i = this.theme.getFormRadioControl(s, this.input, !("horizontal" !== this.options.layout && !this.options.compact));
                        this.radioContainer.appendChild(i)
                    }
                    if (this.schema.readOnly || this.schema.readonly) {
                        this.always_disabled = !0;
                        for (let t = 0; t < this.radioGroup.length; t++) this.radioGroup[t].disabled = !0;
                        this.radioContainer.classList.add("readonly")
                    }
                    const e = this.theme.getContainer();
                    e.appendChild(this.radioContainer), e.dataset.containerFor = "radio", this.input = e, this.control = this.theme.getFormControl(this.label, e, this.description, this.infoButton), this.container.appendChild(this.control), window.requestAnimationFrame(() => {
                        this.input.parentNode && this.afterInputReady()
                    })
                }

                enable() {
                    if (!this.always_disabled) {
                        for (let t = 0; t < this.radioGroup.length; t++) this.radioGroup[t].disabled = !1;
                        this.radioContainer.classList.remove("readonly"), super.enable()
                    }
                }

                disable(t) {
                    t && (this.always_disabled = !0);
                    for (let t = 0; t < this.radioGroup.length; t++) this.radioGroup[t].disabled = !0;
                    this.radioContainer.classList.add("readonly"), super.disable()
                }

                destroy() {
                    this.radioContainer.parentNode && this.radioContainer.parentNode.parentNode && this.radioContainer.parentNode.parentNode.removeChild(this.radioContainer.parentNode), this.label && this.label.parentNode && this.label.parentNode.removeChild(this.label), this.description && this.description.parentNode && this.description.parentNode.removeChild(this.description), super.destroy()
                }

                getNumColumns() {
                    return 2
                }

                setValue(t) {
                    for (let e = 0; e < this.radioGroup.length; e++) if (this.radioGroup[e].value === t) {
                        this.radioGroup[e].checked = !0, this.value = t, this.onChange();
                        break
                    }
                }
            }, sceditor: class extends y {
                setValue(t, e, s) {
                    const i = super.setValue(t, e, s);
                    void 0 !== i && i.changed && this.sceditor_instance && this.sceditor_instance.val(i.value)
                }

                build() {
                    this.options.format = "textarea", super.build(), this.input_type = this.schema.format, this.input.setAttribute("data-schemaformat", this.input_type)
                }

                afterInputReady() {
                    if (window.sceditor) {
                        const t = this.expandCallbacks("sceditor", c({}, {
                                format: this.input_type,
                                emoticonsEnabled: !1,
                                width: "100%",
                                height: 300,
                                readOnly: this.schema.readOnly || this.schema.readonly || this.schema.template
                            }, this.defaults.options.sceditor || {}, this.options.sceditor || {}, {element: this.input})),
                            e = window.sceditor.instance(this.input);
                        void 0 === e && window.sceditor.create(this.input, t), this.sceditor_instance = e || window.sceditor.instance(this.input), this.sceditor_instance.blur(() => {
                            this.value = this.sceditor_instance.val(), this.sceditor_instance.updateOriginal(), this.is_dirty = !0, this.onChange(!0)
                        }), this.theme.afterInputReady(this.input)
                    } else super.afterInputReady()
                }

                getNumColumns() {
                    return 6
                }

                enable() {
                    !this.always_disabled && this.sceditor_instance && this.sceditor_instance.readOnly(!1), super.enable()
                }

                disable(t) {
                    this.sceditor_instance && this.sceditor_instance.readOnly(!0), super.disable(t)
                }

                destroy() {
                    this.sceditor_instance && (this.sceditor_instance.destroy(), this.sceditor_instance = null), super.destroy()
                }
            }, select: x, select2: class extends x {
                setValue(t, e) {
                    if (this.select2_instance) {
                        e ? this.is_dirty = !1 : "change" === this.jsoneditor.options.show_errors && (this.is_dirty = !0);
                        const s = this.updateValue(t);
                        this.input.value = s, this.select2v4 ? this.select2_instance.val(s).trigger("change") : this.select2_instance.select2("val", s), this.onChange(!0)
                    } else super.setValue(t, e)
                }

                afterInputReady() {
                    if (window.jQuery && window.jQuery.fn && window.jQuery.fn.select2 && !this.select2_instance) {
                        const t = this.expandCallbacks("select2", c({}, this.defaults.options.select2 || {}, this.options.select2 || {}));
                        this.newEnumAllowed = t.tags = !!t.tags && "string" === this.schema.type, this.select2_instance = window.jQuery(this.input).select2(t), this.select2v4 = u(this.select2_instance.select2, "amd"), this.selectChangeHandler = () => {
                            const t = this.select2v4 ? this.select2_instance.val() : this.select2_instance.select2("val");
                            this.updateValue(t), this.onChange(!0)
                        }, this.select2_instance.on("change", this.selectChangeHandler), this.select2_instance.on("select2-blur", this.selectChangeHandler)
                    }
                    super.afterInputReady()
                }

                updateValue(t) {
                    let e = this.enum_values[0];
                    return t = this.typecast(t || ""), this.enum_values.includes(t) ? e = t : this.newEnumAllowed && (e = this.addNewOption(t) ? t : e), this.value = e, e
                }

                addNewOption(t) {
                    const e = this.typecast(t);
                    let s, i = !1;
                    return this.enum_values.includes(e) || "" === e || (this.enum_options.push("" + e), this.enum_display.push("" + e), this.enum_values.push(e), this.schema.enum.push(e), s = this.input.querySelector(`option[value="${e}"]`), s ? s.removeAttribute("data-select2-tag") : this.input.appendChild(new Option(e, e, !1, !1)).trigger("change"), i = !0), i
                }

                enable() {
                    this.always_disabled || this.select2_instance && (this.select2v4 ? this.select2_instance.prop("disabled", !1) : this.select2_instance.select2("enable", !0)), super.enable()
                }

                disable(t) {
                    this.select2_instance && (this.select2v4 ? this.select2_instance.prop("disabled", !0) : this.select2_instance.select2("enable", !1)), super.disable(t)
                }

                destroy() {
                    this.select2_instance && (this.select2_instance.select2("destroy"), this.select2_instance = null), super.destroy()
                }
            }, selectize: class extends x {
                setValue(t, e) {
                    if (this.selectize_instance) {
                        e ? this.is_dirty = !1 : "change" === this.jsoneditor.options.show_errors && (this.is_dirty = !0);
                        const s = this.updateValue(t);
                        this.input.value = s, this.selectize_instance.clear(!0), this.selectize_instance.setValue(s), this.onChange(!0)
                    } else super.setValue(t, e)
                }

                afterInputReady() {
                    if (window.jQuery && window.jQuery.fn && window.jQuery.fn.selectize && !this.selectize_instance) {
                        const t = this.expandCallbacks("selectize", c({}, this.defaults.options.selectize || {}, this.options.selectize || {}));
                        this.newEnumAllowed = t.create = !!t.create && "string" === this.schema.type, this.selectize_instance = window.jQuery(this.input).selectize(t)[0].selectize, this.control.removeEventListener("change", this.multiselectChangeHandler), this.multiselectChangeHandler = t => {
                            this.updateValue(t), this.onChange(!0)
                        }, this.selectize_instance.on("change", this.multiselectChangeHandler)
                    }
                    super.afterInputReady()
                }

                updateValue(t) {
                    let e = this.enum_values[0];
                    return t = this.typecast(t || ""), this.enum_values.includes(t) ? e = t : this.newEnumAllowed && (e = this.addNewOption(t) ? t : e), this.value = e, e
                }

                addNewOption(t) {
                    const e = this.typecast(t);
                    let s = !1;
                    return this.enum_values.includes(e) || "" === e || (this.enum_options.push("" + e), this.enum_display.push("" + e), this.enum_values.push(e), this.schema.enum.push(e), this.selectize_instance.addItem(e), this.selectize_instance.refreshOptions(!1), s = !0), s
                }

                onWatchedFieldChange() {
                    super.onWatchedFieldChange(), this.selectize_instance && (this.selectize_instance.clear(!0), this.selectize_instance.clearOptions(!0), this.enum_options.forEach((t, e) => {
                        this.selectize_instance.addOption({value: t, text: this.enum_display[e]})
                    }), this.selectize_instance.addItem("" + this.value, !0))
                }

                enable() {
                    !this.always_disabled && this.selectize_instance && this.selectize_instance.unlock(), super.enable()
                }

                disable(t) {
                    this.selectize_instance && this.selectize_instance.lock(), super.disable(t)
                }

                destroy() {
                    this.selectize_instance && (this.selectize_instance.destroy(), this.selectize_instance = null), super.destroy()
                }
            }, signature: class extends y {
                build() {
                    this.options.compact || (this.header = this.label = this.theme.getFormInputLabel(this.getTitle(), this.isRequired())), this.schema.description && (this.description = this.theme.getFormInputDescription(this.schema.description));
                    const t = this.formname.replace(/\W/g, "");
                    if ("function" == typeof SignaturePad) {
                        this.input = this.theme.getFormInputField("hidden"), this.container.appendChild(this.input);
                        const e = document.createElement("div");
                        e.classList.add("signature-container");
                        const s = document.createElement("canvas");
                        s.setAttribute("name", t), s.classList.add("signature"), e.appendChild(s), this.signaturePad = new window.SignaturePad(s, {
                            onEnd() {
                                this.signaturePad.isEmpty() ? this.input.value = "" : this.input.value = this.signaturePad.toDataURL(), this.is_dirty = !0, this.refreshValue(), this.watch_listener(), this.jsoneditor.notifyWatchers(this.path), this.parent ? this.parent.onChildEditorChange(this) : this.jsoneditor.onChange()
                            }
                        });
                        const i = document.createElement("div"), r = document.createElement("button");
                        r.classList.add("tiny", "button"), r.innerHTML = "Clear signature", i.appendChild(r), e.appendChild(i), this.options.compact && this.container.setAttribute("class", this.container.getAttribute("class") + " compact"), (this.schema.readOnly || this.schema.readonly) && (this.always_disabled = !0, Array.from(this.inputs).forEach(t => {
                            s.setAttribute("readOnly", "readOnly"), t.disabled = !0
                        })), r.addEventListener("click", t => {
                            t.preventDefault(), t.stopPropagation(), this.signaturePad.clear(), this.signaturePad.strokeEnd()
                        }), this.control = this.theme.getFormControl(this.label, e, this.description), this.container.appendChild(this.control), this.refreshValue(), s.width = e.offsetWidth, this.options && this.options.canvas_height ? s.height = this.options.canvas_height : s.height = "300"
                    } else {
                        const t = document.createElement("p");
                        t.innerHTML = "Signature pad is not available, please include SignaturePad from https://github.com/szimek/signature_pad", this.container.appendChild(t)
                    }
                }

                setValue(t) {
                    if ("function" == typeof SignaturePad) {
                        const e = this.sanitize(t);
                        if (this.value === e) return;
                        return this.value = e, this.input.value = this.value, this.signaturePad.clear(), t && "" !== t && this.signaturePad.fromDataURL(t), this.watch_listener(), this.jsoneditor.notifyWatchers(this.path), !1
                    }
                }

                destroy() {
                    this.signaturePad.off(), delete this.signaturePad
                }
            }, simplemde: class extends y {
                setValue(t, e, s) {
                    const i = super.setValue(t, e, s);
                    void 0 !== i && i.changed && this.simplemde_instance && this.simplemde_instance.value(i.value)
                }

                build() {
                    this.options.format = "textarea", super.build(), this.input_type = this.schema.format, this.input.setAttribute("data-schemaformat", this.input_type)
                }

                afterInputReady() {
                    let t;
                    window.SimpleMDE ? (t = this.expandCallbacks("simplemde", c({}, {height: 300}, this.defaults.options.simplemde || {}, this.options.simplemde || {}, {element: this.input})), this.simplemde_instance = new window.SimpleMDE(t), (this.schema.readOnly || this.schema.readonly || this.schema.template) && (this.simplemde_instance.codemirror.options.readOnly = !0), this.simplemde_instance.codemirror.on("change", () => {
                        this.value = this.simplemde_instance.value(), this.is_dirty = !0, this.onChange(!0)
                    }), t.autorefresh && this.startListening(this.simplemde_instance.codemirror, this.simplemde_instance.codemirror.state.autoRefresh = {delay: 250}), this.theme.afterInputReady(this.input)) : super.afterInputReady()
                }

                getNumColumns() {
                    return 6
                }

                enable() {
                    !this.always_disabled && this.simplemde_instance && (this.simplemde_instance.codemirror.options.readOnly = !1), super.enable()
                }

                disable(t) {
                    this.simplemde_instance && (this.simplemde_instance.codemirror.options.readOnly = !0), super.disable(t)
                }

                destroy() {
                    this.simplemde_instance && (this.simplemde_instance.toTextArea(), this.simplemde_instance = null), super.destroy()
                }

                startListening(t, e) {
                    function s() {
                        t.display.wrapper.offsetHeight ? (this.stopListening(t, e), t.display.lastWrapHeight !== t.display.wrapper.clientHeight && t.refresh()) : e.timeout = window.setTimeout(s, e.delay)
                    }

                    e.timeout = window.setTimeout(s, e.delay), e.hurry = () => {
                        window.clearTimeout(e.timeout), e.timeout = window.setTimeout(s, 50)
                    }, t.on(window, "mouseup", e.hurry), t.on(window, "keyup", e.hurry)
                }

                stopListening(t, e) {
                    window.clearTimeout(e.timeout), t.off(window, "mouseup", e.hurry), t.off(window, "keyup", e.hurry)
                }
            }, starrating: j, string: y, table: class extends w {
                register() {
                    if (super.register(), this.rows) for (let t = 0; t < this.rows.length; t++) this.rows[t].register()
                }

                unregister() {
                    if (super.unregister(), this.rows) for (let t = 0; t < this.rows.length; t++) this.rows[t].unregister()
                }

                getNumColumns() {
                    return Math.max(Math.min(12, this.width), 3)
                }

                preBuild() {
                    const t = this.jsoneditor.expandRefs(this.schema.items || {});
                    this.item_title = t.title || "row", this.item_default = t.default || null, this.item_has_child_editors = t.properties || t.items, this.width = 12, super.preBuild()
                }

                build() {
                    this.table = this.theme.getTable(), this.container.appendChild(this.table), this.thead = this.theme.getTableHead(), this.table.appendChild(this.thead), this.header_row = this.theme.getTableRow(), this.thead.appendChild(this.header_row), this.row_holder = this.theme.getTableBody(), this.table.appendChild(this.row_holder);
                    const t = this.getElementEditor(0, !0);
                    if (this.item_default = t.getDefault(), this.width = t.getNumColumns() + 2, this.options.compact ? (this.panel = document.createElement("div"), this.container.appendChild(this.panel)) : (this.header = document.createElement("label"), this.header.textContent = this.getTitle(), this.title = this.theme.getHeader(this.header), this.container.appendChild(this.title), this.title_controls = this.theme.getHeaderButtonHolder(), this.title.appendChild(this.title_controls), this.schema.description && (this.description = this.theme.getDescription(this.schema.description), this.container.appendChild(this.description)), this.panel = this.theme.getIndentedPanel(), this.container.appendChild(this.panel), this.error_holder = document.createElement("div"), this.panel.appendChild(this.error_holder)), this.panel.appendChild(this.table), this.controls = this.theme.getButtonHolder(), this.panel.appendChild(this.controls), this.item_has_child_editors) {
                        const e = t.getChildEditors(), s = t.property_order || Object.keys(e);
                        for (let t = 0; t < s.length; t++) {
                            const i = this.theme.getTableHeaderCell(e[s[t]].getTitle());
                            e[s[t]].options.hidden && (i.style.display = "none"), this.header_row.appendChild(i)
                        }
                    } else this.header_row.appendChild(this.theme.getTableHeaderCell(this.item_title));
                    t.destroy(), this.row_holder.innerHTML = "", this.controls_header_cell = this.theme.getTableHeaderCell(" "), this.header_row.appendChild(this.controls_header_cell), this.addControls()
                }

                onChildEditorChange(t) {
                    this.refreshValue(), super.onChildEditorChange()
                }

                getItemDefault() {
                    return c({}, {default: this.item_default}).default
                }

                getItemTitle() {
                    return this.item_title
                }

                getElementEditor(t, e) {
                    const s = c({}, this.schema.items), i = this.jsoneditor.getEditorClass(s, this.jsoneditor),
                        r = this.row_holder.appendChild(this.theme.getTableRow());
                    let o = r;
                    this.item_has_child_editors || (o = this.theme.getTableCell(), r.appendChild(o));
                    const n = this.jsoneditor.createEditor(i, {
                        jsoneditor: this.jsoneditor,
                        schema: s,
                        container: o,
                        path: `${this.path}.${t}`,
                        parent: this,
                        compact: !0,
                        table_row: !0
                    });
                    return n.preBuild(), e || (n.build(), n.postBuild(), n.controls_cell = r.appendChild(this.theme.getTableCell()), n.row = r, n.table_controls = this.theme.getButtonHolder(), n.controls_cell.appendChild(n.table_controls), n.table_controls.style.margin = 0, n.table_controls.style.padding = 0), n
                }

                destroy() {
                    this.innerHTML = "", this.title && this.title.parentNode && this.title.parentNode.removeChild(this.title), this.description && this.description.parentNode && this.description.parentNode.removeChild(this.description), this.row_holder && this.row_holder.parentNode && this.row_holder.parentNode.removeChild(this.row_holder), this.table && this.table.parentNode && this.table.parentNode.removeChild(this.table), this.panel && this.panel.parentNode && this.panel.parentNode.removeChild(this.panel), this.rows = this.title = this.description = this.row_holder = this.table = this.panel = null, super.destroy()
                }

                setValue(t = [], e) {
                    if (this.schema.minItems) for (; t.length < this.schema.minItems;) t.push(this.getItemDefault());
                    if (this.schema.maxItems && t.length > this.schema.maxItems && (t = t.slice(0, this.schema.maxItems)), JSON.stringify(t) === this.serialized) return;
                    let s = !1;
                    t.forEach((t, e) => {
                        this.rows[e] ? this.rows[e].setValue(t) : (this.addRow(t), s = !0)
                    });
                    for (let e = t.length; e < this.rows.length; e++) {
                        const t = this.rows[e].container;
                        this.item_has_child_editors || this.rows[e].row.parentNode.removeChild(this.rows[e].row), this.rows[e].destroy(), t.parentNode && t.parentNode.removeChild(t), this.rows[e] = null, s = !0
                    }
                    this.rows = this.rows.slice(0, t.length), this.refreshValue(), (s || e) && this.refreshRowButtons(), this.onChange()
                }

                refreshRowButtons() {
                    const t = this.schema.minItems && this.schema.minItems >= this.rows.length,
                        e = this.schema.maxItems && this.schema.maxItems <= this.rows.length;
                    let s = !1;
                    this.rows.forEach((i, r) => {
                        i.delete_button && (t ? i.delete_button.style.display = "none" : (s = !0, i.delete_button.style.display = "")), i.copy_button && (e ? i.copy_button.style.display = "none" : (s = !0, i.copy_button.style.display = "")), i.moveup_button && (0 === r ? i.moveup_button.style.display = "none" : (s = !0, i.moveup_button.style.display = "")), i.movedown_button && (r === this.rows.length - 1 ? i.movedown_button.style.display = "none" : (s = !0, i.movedown_button.style.display = ""))
                    }), this.rows.forEach(t => {
                        t.controls_cell.style.display = s ? "" : "none"
                    }), this.controls_header_cell.style.display = s ? "" : "none", this.value.length ? this.table.style.display = "" : this.table.style.display = "none";
                    let i = !1;
                    e || this.hide_add_button ? this.add_row_button.style.display = "none" : (this.add_row_button.style.display = "", i = !0), !this.value.length || t || this.hide_delete_last_row_buttons ? this.delete_last_row_button.style.display = "none" : (this.delete_last_row_button.style.display = "", i = !0), this.value.length <= 1 || t || this.hide_delete_all_rows_buttons ? this.remove_all_rows_button.style.display = "none" : (this.remove_all_rows_button.style.display = "", i = !0), this.controls.style.display = i ? "" : "none"
                }

                refreshValue() {
                    this.value = [], this.rows.forEach((t, e) => {
                        this.value[e] = t.getValue()
                    }), this.serialized = JSON.stringify(this.value)
                }

                addRow(t) {
                    const e = this.rows.length;
                    this.rows[e] = this.getElementEditor(e);
                    const s = this.rows[e].table_controls;
                    this.hide_delete_buttons || (this.rows[e].delete_button = this.getButton("", "delete", this.translate("button_delete_row_title_short")), this.rows[e].delete_button.classList.add("delete", "json-editor-btntype-delete"), this.rows[e].delete_button.setAttribute("data-i", e), this.rows[e].delete_button.addEventListener("click", t => {
                        if (t.preventDefault(), t.stopPropagation(), !this.askConfirmation()) return !1;
                        const e = 1 * t.currentTarget.getAttribute("data-i"), s = this.getValue();
                        s.splice(e, 1), this.setValue(s), this.onChange(!0), this.jsoneditor.trigger("deleteRow", this.rows[e])
                    }), s.appendChild(this.rows[e].delete_button)), this.show_copy_button && (this.rows[e].copy_button = this.getButton("", "copy", this.translate("button_copy_row_title_short")), this.rows[e].copy_button.classList.add("copy", "json-editor-btntype-copy"), this.rows[e].copy_button.setAttribute("data-i", e), this.rows[e].copy_button.addEventListener("click", t => {
                        t.preventDefault(), t.stopPropagation();
                        const e = 1 * t.currentTarget.getAttribute("data-i"), s = this.getValue();
                        s.splice(e + 1, 0, s[e]), this.setValue(s), this.onChange(!0), this.jsoneditor.trigger("copyRow", this.rows[e + 1])
                    }), s.appendChild(this.rows[e].copy_button)), this.hide_move_buttons || (this.rows[e].moveup_button = this.getButton("", "moveup", this.translate("button_move_up_title")), this.rows[e].moveup_button.classList.add("moveup", "json-editor-btntype-move"), this.rows[e].moveup_button.setAttribute("data-i", e), this.rows[e].moveup_button.addEventListener("click", t => {
                        t.preventDefault(), t.stopPropagation();
                        const e = 1 * t.currentTarget.getAttribute("data-i"), s = this.getValue();
                        s.splice(e - 1, 0, s.splice(e, 1)[0]), this.setValue(s), this.onChange(!0), this.jsoneditor.trigger("moveRow", this.rows[e - 1])
                    }), s.appendChild(this.rows[e].moveup_button)), this.hide_move_buttons || (this.rows[e].movedown_button = this.getButton("", "movedown", this.translate("button_move_down_title")), this.rows[e].movedown_button.classList.add("movedown", "json-editor-btntype-move"), this.rows[e].movedown_button.setAttribute("data-i", e), this.rows[e].movedown_button.addEventListener("click", t => {
                        t.preventDefault(), t.stopPropagation();
                        const e = 1 * t.currentTarget.getAttribute("data-i"), s = this.getValue();
                        s.splice(e + 1, 0, s.splice(e, 1)[0]), this.setValue(s), this.onChange(!0), this.jsoneditor.trigger("moveRow", this.rows[e + 1])
                    }), s.appendChild(this.rows[e].movedown_button)), t && this.rows[e].setValue(t)
                }

                addControls() {
                    this.collapsed = !1, this.toggle_button = this.getButton("", "collapse", this.translate("button_collapse")), this.toggle_button.classList.add("json-editor-btntype-toggle"), this.toggle_button.style.margin = "0 10px 0 0", this.title_controls && (this.title.insertBefore(this.toggle_button, this.title.childNodes[0]), this.toggle_button.addEventListener("click", t => {
                        t.preventDefault(), t.stopPropagation(), this.collapsed ? (this.collapsed = !1, this.panel.style.display = "", this.setButtonText(t.currentTarget, "", "collapse", this.translate("button_collapse"))) : (this.collapsed = !0, this.panel.style.display = "none", this.setButtonText(t.currentTarget, "", "expand", this.translate("button_expand")))
                    }), this.options.collapsed && p(this.toggle_button, "click"), this.schema.options && void 0 !== this.schema.options.disable_collapse ? this.schema.options.disable_collapse && (this.toggle_button.style.display = "none") : this.jsoneditor.options.disable_collapse && (this.toggle_button.style.display = "none")), this.add_row_button = this.getButton(this.getItemTitle(), "add", this.translate("button_add_row_title", [this.getItemTitle()])), this.add_row_button.classList.add("json-editor-btntype-add"), this.add_row_button.addEventListener("click", t => {
                        t.preventDefault(), t.stopPropagation();
                        const e = this.addRow();
                        this.refreshValue(), this.refreshRowButtons(), this.onChange(!0), this.jsoneditor.trigger("addRow", e)
                    }), this.controls.appendChild(this.add_row_button), this.delete_last_row_button = this.getButton(this.translate("button_delete_last", [this.getItemTitle()]), "subtract", this.translate("button_delete_last_title", [this.getItemTitle()])), this.delete_last_row_button.classList.add("json-editor-btntype-deletelast"), this.delete_last_row_button.addEventListener("click", t => {
                        if (t.preventDefault(), t.stopPropagation(), !this.askConfirmation()) return !1;
                        const e = this.getValue(), s = e.pop();
                        this.setValue(e), this.onChange(!0), this.jsoneditor.trigger("deleteRow", s)
                    }), this.controls.appendChild(this.delete_last_row_button), this.remove_all_rows_button = this.getButton(this.translate("button_delete_all"), "delete", this.translate("button_delete_all_title")), this.remove_all_rows_button.classList.add("json-editor-btntype-deleteall"), this.remove_all_rows_button.addEventListener("click", t => {
                        if (t.preventDefault(), t.stopPropagation(), !this.askConfirmation()) return !1;
                        this.setValue([]), this.onChange(!0), this.jsoneditor.trigger("deleteAllRows")
                    }), this.controls.appendChild(this.remove_all_rows_button)
                }
            }, upload: class extends _ {
                getNumColumns() {
                    return 4
                }

                build() {
                    if (this.options.compact || (this.header = this.label = this.theme.getFormInputLabel(this.getTitle(), this.isRequired())), this.schema.description && (this.description = this.theme.getFormInputDescription(this.schema.description)), this.options.infoText && (this.infoButton = this.theme.getInfoButton(this.options.infoText)), this.options = this.expandCallbacks("upload", c({}, {
                        title: "Browse",
                        icon: "",
                        auto_upload: !1,
                        hide_input: !1,
                        enable_drag_drop: !1,
                        drop_zone_text: "Drag & Drop file here",
                        drop_zone_top: !1,
                        alt_drop_zone: "",
                        mime_type: "",
                        max_upload_size: 0,
                        upload_handler: (t, e, s, i) => {
                            window.alert(`No upload_handler defined for "${t.path}". You must create your own handler to enable upload to server`)
                        }
                    }, this.defaults.options.upload || {}, this.options.upload || {})), this.options.mime_type = this.options.mime_type ? [].concat(this.options.mime_type) : [], this.input = this.theme.getFormInputField("hidden"), this.container.appendChild(this.input), !this.schema.readOnly && !this.schema.readonly) {
                        if ("function" != typeof this.options.upload_handler) throw new Error("Upload handler required for upload editor");
                        if (this.uploader = this.theme.getFormInputField("file"), this.uploader.style.display = "none", this.options.mime_type.length && this.uploader.setAttribute("accept", this.options.mime_type), !0 === this.options.enable_drag_drop && !0 === this.options.hide_input || (this.clickHandler = t => {
                            this.uploader.dispatchEvent(new window.MouseEvent("click", {
                                view: window,
                                bubbles: !0,
                                cancelable: !1
                            }))
                        }, this.browseButton = this.getButton(this.options.title, this.options.icon, this.options.title), this.browseButton.addEventListener("click", this.clickHandler), this.fileDisplay = this.theme.getFormInputField("input"), this.fileDisplay.setAttribute("readonly", !0), this.fileDisplay.value = "No file selected.", this.fileDisplay.addEventListener("dblclick", this.clickHandler), this.fileUploadGroup = this.theme.getInputGroup(this.fileDisplay, [this.browseButton]), this.fileUploadGroup || (this.fileUploadGroup = document.createElement("div"), this.fileUploadGroup.appendChild(this.fileDisplay), this.fileUploadGroup.appendChild(this.browseButton))), !0 === this.options.enable_drag_drop) {
                            if ("" !== this.options.alt_drop_zone) {
                                if (this.altDropZone = document.querySelector(this.options.alt_drop_zone), !this.altDropZone) throw new Error(`Error: alt_drop_zone selector "${this.options.alt_drop_zone}" not found!`);
                                this.dropZone = this.altDropZone
                            } else this.dropZone = this.theme.getDropZone(this.options.drop_zone_text);
                            this.dropZone && (this.dropZone.classList.add("upload-dropzone"), this.dropZone.addEventListener("dblclick", this.clickHandler))
                        }
                        this.uploadHandler = t => {
                            t.preventDefault(), t.stopPropagation();
                            const e = t.target.files || t.dataTransfer.files;
                            if (e && e.length) if (0 !== this.options.max_upload_size && e[0].size > this.options.max_upload_size) this.theme.addInputError(this.uploader, "Filesize too large. Max size is " + this.options.max_upload_size); else if (0 === this.options.mime_type.length || this.isValidMimeType(e[0].type, this.options.mime_type)) {
                                this.fileDisplay && (this.fileDisplay.value = e[0].name);
                                let t = new window.FileReader;
                                t.onload = s => {
                                    this.preview_value = s.target.result, this.refreshPreview(e), this.onChange(!0), t = null
                                }, t.readAsDataURL(e[0])
                            } else this.theme.addInputError(this.uploader, "Wrong file format. Allowed format(s): " + this.options.mime_type.toString())
                        }, this.uploader.addEventListener("change", this.uploadHandler), this.dragHandler = t => {
                            const e = t.dataTransfer.items || t.dataTransfer.files,
                                s = e && e.length && (0 === this.options.mime_type.length || this.isValidMimeType(e[0].type, this.options.mime_type)),
                                i = t.currentTarget.classList && t.currentTarget.classList.contains("upload-dropzone") && s;
                            switch ((t.currentTarget === window ? "w_" : "e_") + t.type) {
                                case"w_drop":
                                case"w_dragover":
                                    i || (t.dataTransfer.dropEffect = "none");
                                    break;
                                case"e_dragenter":
                                    i ? (this.dropZone.classList.add("valid-dropzone"), t.dataTransfer.dropEffect = "copy") : this.dropZone.classList.add("invalid-dropzone");
                                    break;
                                case"e_dragover":
                                    i && (t.dataTransfer.dropEffect = "copy");
                                    break;
                                case"e_dragleave":
                                    this.dropZone.classList.remove("valid-dropzone", "invalid-dropzone");
                                    break;
                                case"e_drop":
                                    this.dropZone.classList.remove("valid-dropzone", "invalid-dropzone"), i && this.uploadHandler(t)
                            }
                            i || t.preventDefault()
                        }, !0 === this.options.enable_drag_drop && (["dragover", "drop"].forEach(t => {
                            window.addEventListener(t, this.dragHandler, !0)
                        }), ["dragenter", "dragover", "dragleave", "drop"].forEach(t => {
                            this.dropZone.addEventListener(t, this.dragHandler, !0)
                        }))
                    }
                    this.preview = document.createElement("div"), this.control = this.input.controlgroup = this.theme.getFormControl(this.label, this.uploader || this.input, this.description, this.infoButton), this.uploader && (this.uploader.controlgroup = this.control);
                    const t = this.uploader || this.input, e = document.createElement("div");
                    this.dropZone && !this.altDropZone && !0 === this.options.drop_zone_top && e.appendChild(this.dropZone), this.fileUploadGroup && e.appendChild(this.fileUploadGroup), this.dropZone && !this.altDropZone && !0 !== this.options.drop_zone_top && e.appendChild(this.dropZone), e.appendChild(this.preview), t.parentNode.insertBefore(e, t.nextSibling), this.container.appendChild(this.control), window.requestAnimationFrame(() => {
                        this.afterInputReady()
                    })
                }

                afterInputReady() {
                    if (this.value) {
                        const t = document.createElement("img");
                        t.style.maxWidth = "100%", t.style.maxHeight = "100px", t.onload = e => {
                            this.preview.appendChild(t)
                        }, t.onerror = t => {
                            console.error("upload error", t, t.currentTarget)
                        }, t.src = this.container.querySelector("a").href
                    }
                    this.theme.afterInputReady(this.input)
                }

                refreshPreview(t) {
                    if (this.last_preview === this.preview_value) return;
                    if (this.last_preview = this.preview_value, this.preview.innerHTML = "", !this.preview_value) return;
                    const e = t[0], s = this.preview_value.match(/^data:([^;,]+)[;,]/);
                    if (e.mimeType = s ? s[1] : "unknown", e.size > 0) {
                        const t = Math.floor(Math.log(e.size) / Math.log(1024));
                        e.formattedSize = `${parseFloat((e.size / 1024 ** t).toFixed(2))} ${["Bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"][t]}`
                    } else e.formattedSize = "0 Bytes";
                    const i = this.getButton("Upload", "upload", "Upload");
                    i.addEventListener("click", t => {
                        t.preventDefault(), i.setAttribute("disabled", "disabled"), this.theme.removeInputError(this.uploader), this.theme.getProgressBar && (this.progressBar = this.theme.getProgressBar(), this.preview.appendChild(this.progressBar)), this.options.upload_handler(this.path, e, {
                            success(t) {
                                this.setValue(t), this.parent ? this.parent.onChildEditorChange(this) : this.jsoneditor.onChange(), this.progressBar && this.preview.removeChild(this.progressBar), i.removeAttribute("disabled")
                            }, failure(t) {
                                this.theme.addInputError(this.uploader, t), this.progressBar && this.preview.removeChild(this.progressBar), i.removeAttribute("disabled")
                            }, updateProgress(t) {
                                this.progressBar && (t ? this.theme.updateProgressBar(this.progressBar, t) : this.theme.updateProgressBarUnknown(this.progressBar))
                            }
                        })
                    }), this.preview.appendChild(this.theme.getUploadPreview(e, i, this.preview_value)), this.options.auto_upload && (i.dispatchEvent(new window.MouseEvent("click")), this.preview.removeChild(i))
                }

                enable() {
                    this.always_disabled || (this.uploader && (this.uploader.disabled = !1), super.enable())
                }

                disable(t) {
                    t && (this.always_disabled = !0), this.uploader && (this.uploader.disabled = !0), super.disable()
                }

                setValue(t) {
                    this.value !== t && (this.value = t, this.input.value = this.value, this.onChange())
                }

                destroy() {
                    !0 === this.options.enable_drag_drop && (["dragover", "drop"].forEach(t => {
                        window.removeEventListener(t, this.dragHandler, !0)
                    }), ["dragenter", "dragover", "dragleave", "drop"].forEach(t => {
                        this.dropZone.removeEventListener(t, this.dragHandler, !0)
                    }), this.dropZone.removeEventListener("dblclick", this.clickHandler), this.dropZone && this.dropZone.parentNode && this.dropZone.parentNode.removeChild(this.dropZone)), this.uploader && this.uploader.parentNode && (this.uploader.removeEventListener("change", this.uploadHandler), this.uploader.parentNode.removeChild(this.uploader)), this.browseButton && this.browseButton.parentNode && (this.browseButton.removeEventListener("click", this.clickHandler), this.browseButton.parentNode.removeChild(this.browseButton)), this.fileDisplay && this.fileDisplay.parentNode && (this.fileDisplay.removeEventListener("dblclick", this.clickHandler), this.fileDisplay.parentNode.removeChild(this.fileDisplay)), this.fileUploadGroup && this.fileUploadGroup.parentNode && this.fileUploadGroup.parentNode.removeChild(this.fileUploadGroup), this.preview && this.preview.parentNode && this.preview.parentNode.removeChild(this.preview), this.header && this.header.parentNode && this.header.parentNode.removeChild(this.header), this.input && this.input.parentNode && this.input.parentNode.removeChild(this.input), super.destroy()
                }

                isValidMimeType(t, e) {
                    return e.reduce((e, s) => e || new RegExp(s.replace(/\*/g, ".*"), "gi").test(t), !1)
                }
            }, uuid: class extends _ {
                preBuild() {
                    super.preBuild(), this.schema.default = this.uuid = this.getUuid(), this.jsoneditor.validator.schema.properties[this.key].pattern = this.schema.pattern = "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}$", this.schema.options || (this.schema.options = {}), this.schema.options.cleave || (this.schema.options.cleave = {
                        delimiters: ["-"],
                        blocks: [8, 4, 4, 4, 12]
                    })
                }

                sanitize(t) {
                    return this.testUuid(t) || (t = this.uuid), t
                }

                setValue(t, e, s) {
                    this.testUuid(t) || (t = this.uuid), this.uuid = t, super.setValue(t, e, s)
                }

                getUuid() {
                    let t = (new Date).getTime();
                    return "undefined" != typeof performance && "function" == typeof performance.now && (t += performance.now()), "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx".replace(/[xy]/g, e => {
                        const s = (t + 16 * Math.random()) % 16 | 0;
                        return t = Math.floor(t / 16), ("x" === e ? s : 3 & s | 8).toString(16)
                    })
                }

                testUuid(t) {
                    return /^[0-9a-f]{8}-[0-9a-f]{4}-[1-5][0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i.test(t)
                }
            }, colorpicker: class extends y {
                postBuild() {
                    window.Picker && (this.input.type = "text"), this.input.style.padding = "3px"
                }

                setValue(t, e, s) {
                    const i = super.setValue(t, e, s);
                    return this.picker_instance && this.picker_instance.domElement && i && i.changed && this.picker_instance.setColor(i.value, !0), i
                }

                getNumColumns() {
                    return 2
                }

                afterInputReady() {
                    super.afterInputReady(), this.createPicker(!0)
                }

                disable() {
                    if (super.disable(), this.picker_instance && this.picker_instance.domElement) {
                        this.picker_instance.domElement.style.pointerEvents = "none";
                        const t = this.picker_instance.domElement.querySelectorAll("button");
                        for (let e = 0; e < t.length; e++) t[e].disabled = !0
                    }
                }

                enable() {
                    if (super.enable(), this.picker_instance && this.picker_instance.domElement) {
                        this.picker_instance.domElement.style.pointerEvents = "auto";
                        const t = this.picker_instance.domElement.querySelectorAll("button");
                        for (let e = 0; e < t.length; e++) t[e].disabled = !1
                    }
                }

                destroy() {
                    this.createPicker(!1), super.destroy()
                }

                createPicker(t) {
                    if (t) {
                        if (window.Picker && !this.picker_instance) {
                            const t = this.expandCallbacks("colorpicker", c({}, {
                                    editor: !1,
                                    alpha: !1,
                                    color: this.value,
                                    popup: "bottom"
                                }, this.defaults.options.colorpicker || {}, this.options.colorpicker || {}, {parent: this.container})),
                                e = t => {
                                    const e = this.picker_instance.settings.editorFormat,
                                        s = this.picker_instance.settings.alpha;
                                    this.setValue("hex" === e ? s ? t.hex : t.hex.slice(0, 7) : t[e + (s ? "a" : "") + "String"])
                                };
                            t.popup || "function" == typeof t.onChange ? t.popup && "function" != typeof t.onDone && (t.onDone = e) : t.onChange = e, this.picker_instance = new window.Picker(t), t.popup || (this.input.style.display = "none", this.theme.afterInputReady(this.picker_instance.domElement))
                        }
                    } else this.picker_instance && (this.picker_instance.destroy(), this.picker_instance = null, this.input.style.display = "")
                }
            }
        }, T = {
            default: () => ({
                compile(t) {
                    const e = t.match(/{{\s*([a-zA-Z0-9\-_ .]+)\s*}}/g), s = e && e.length;
                    if (!s) return () => t;
                    const i = [], r = t => {
                        let s = e[t].replace(/[{}]+/g, "").trim().split(".");
                        const r = s.length;
                        let o;
                        if (r > 1) {
                            let e;
                            o = i => {
                                for (e = i, t = 0; t < r && (e = e[s[t]], e); t++) ;
                                return e
                            }
                        } else s = s[0], o = t => t[s];
                        i.push({s: e[t], r: o})
                    };
                    for (var o = 0; o < s; o++) r(o);
                    return e => {
                        let r, n = "" + t;
                        for (o = 0; o < s; o++) r = i[o], n = n.replace(r.s, r.r(e));
                        return n
                    }
                }
            }),
            ejs: () => !!window.EJS && {
                compile(t) {
                    const e = new window.EJS({text: t});
                    return t => e.render(t)
                }
            },
            handlebars: () => window.Handlebars,
            hogan: () => !!window.Hogan && {
                compile(t) {
                    const e = window.Hogan.compile(t);
                    return t => e.render(t)
                }
            },
            lodash: () => !!window._ && {compile: t => e => window._.template(t)(e)},
            markup: () => !(!window.Mark || !window.Mark.up) && {compile: t => e => window.Mark.up(t, e)},
            mustache: () => !!window.Mustache && {compile: t => e => window.Mustache.render(t, e)},
            swig: () => window.swig,
            underscore: () => !!window._ && {compile: t => e => window._.template(t)(e)}
        }, A = {
            collapse: "",
            expand: "",
            delete: "",
            edit: "",
            add: "",
            cancel: "",
            save: "",
            moveup: "",
            movedown: ""
        };

        class I {
            constructor(t = "", e = A) {
                this.mapping = e, this.icon_prefix = t
            }

            getIconClass(t) {
                return this.mapping[t] ? this.icon_prefix + this.mapping[t] : null
            }

            getIcon(t) {
                const e = this.getIconClass(t);
                if (!e) return null;
                const s = document.createElement("i");
                return s.classList.add(...e.split(" ")), s
            }
        }

        const S = {
            collapse: "chevron-down",
            expand: "chevron-right",
            delete: "trash",
            edit: "pencil",
            add: "plus",
            subtract: "minus",
            cancel: "floppy-remove",
            save: "floppy-saved",
            moveup: "arrow-up",
            moveright: "arrow-right",
            movedown: "arrow-down",
            moveleft: "arrow-left",
            copy: "copy",
            clear: "remove-circle",
            time: "time",
            calendar: "calendar",
            edit_properties: "list"
        };
        const O = {
            collapse: "chevron-down",
            expand: "chevron-right",
            delete: "trash",
            edit: "pencil",
            add: "plus",
            subtract: "minus",
            cancel: "ban-circle",
            save: "save",
            moveup: "arrow-up",
            moveright: "arrow-right",
            movedown: "arrow-down",
            moveleft: "arrow-left",
            copy: "copy",
            clear: "remove-circle",
            time: "time",
            calendar: "calendar",
            edit_properties: "list"
        };
        const B = {
            collapse: "caret-square-o-down",
            expand: "caret-square-o-right",
            delete: "times",
            edit: "pencil",
            add: "plus",
            subtract: "minus",
            cancel: "ban",
            save: "save",
            moveup: "arrow-up",
            moveright: "arrow-right",
            movedown: "arrow-down",
            moveleft: "arrow-left",
            copy: "files-o",
            clear: "times-circle-o",
            time: "clock-o",
            calendar: "calendar",
            edit_properties: "list"
        };
        const N = {
            collapse: "caret-down",
            expand: "caret-right",
            delete: "trash",
            edit: "pen",
            add: "plus",
            subtract: "minus",
            cancel: "ban",
            save: "save",
            moveup: "arrow-up",
            moveright: "arrow-right",
            movedown: "arrow-down",
            moveleft: "arrow-left",
            copy: "copy",
            clear: "times-circle",
            time: "clock",
            calendar: "calendar",
            edit_properties: "list"
        };
        const F = {
            collapse: "triangle-1-s",
            expand: "triangle-1-e",
            delete: "trash",
            edit: "pencil",
            add: "plusthick",
            subtract: "minusthick",
            cancel: "closethick",
            save: "disk",
            moveup: "arrowthick-1-n",
            moveright: "arrowthick-1-e",
            movedown: "arrowthick-1-s",
            moveleft: "arrowthick-1-w",
            copy: "copy",
            clear: "circle-close",
            time: "time",
            calendar: "calendar",
            edit_properties: "note"
        };
        const V = {
            collapse: "arrow-down",
            expand: "arrow-right",
            delete: "delete",
            edit: "edit",
            add: "plus",
            subtract: "minus",
            cancel: "cross",
            save: "check",
            moveup: "upward",
            moveright: "forward",
            movedown: "downward",
            moveleft: "back",
            copy: "copy",
            clear: "close",
            time: "time",
            calendar: "bookmark",
            edit_properties: "menu"
        };
        const H = {
                bootstrap3: class extends I {
                    constructor() {
                        super("glyphicon glyphicon-", S)
                    }
                }, fontawesome3: class extends I {
                    constructor() {
                        super("icon-", O)
                    }
                }, fontawesome4: class extends I {
                    constructor() {
                        super("fa fa-", B)
                    }
                }, fontawesome5: class extends I {
                    constructor() {
                        super("fas fa-", N)
                    }
                }, jqueryui: class extends I {
                    constructor() {
                        super("ui-icon ui-icon-", F)
                    }
                }, spectre: class extends I {
                    constructor() {
                        super("icon icon-", V)
                    }
                }
            },
            R = ["matches", "webkitMatchesSelector", "mozMatchesSelector", "msMatchesSelector", "oMatchesSelector"].find(t => t in document.documentElement);

        class P {
            constructor(t, e = {disable_theme_rules: !1}) {
                this.jsoneditor = t, this.options = e
            }

            getContainer() {
                return document.createElement("div")
            }

            getFloatRightLinkHolder() {
                const t = document.createElement("div");
                return t.style = t.style || {}, t.style.cssFloat = "right", t.style.marginLeft = "10px", t
            }

            getModal() {
                const t = document.createElement("div");
                return t.style.backgroundColor = "white", t.style.border = "1px solid black", t.style.boxShadow = "3px 3px black", t.style.position = "absolute", t.style.zIndex = "10", t.style.display = "none", t
            }

            getGridContainer() {
                return document.createElement("div")
            }

            getGridRow() {
                const t = document.createElement("div");
                return t.classList.add("row"), t
            }

            getGridColumn() {
                return document.createElement("div")
            }

            setGridColumnSize(t, e) {
            }

            getLink(t) {
                const e = document.createElement("a");
                return e.setAttribute("href", "#"), e.appendChild(document.createTextNode(t)), e
            }

            disableHeader(t) {
                t.style.color = "#ccc"
            }

            disableLabel(t) {
                t.style.color = "#ccc"
            }

            enableHeader(t) {
                t.style.color = ""
            }

            enableLabel(t) {
                t.style.color = ""
            }

            getInfoButton(t) {
                const e = document.createElement("span");
                e.innerText = "ⓘ", e.style.fontSize = "16px", e.style.fontWeight = "bold", e.style.padding = ".25rem", e.style.position = "relative", e.style.display = "inline-block";
                const s = document.createElement("span");
                return s.style.fontSize = "12px", e.style.fontWeight = "normal", s.style["font-family"] = "sans-serif", s.style.visibility = "hidden", s.style["background-color"] = "rgba(50, 50, 50, .75)", s.style.margin = "0 .25rem", s.style.color = "#FAFAFA", s.style.padding = ".5rem 1rem", s.style["border-radius"] = ".25rem", s.style.width = "20rem", s.style.position = "absolute", s.innerText = t, e.onmouseover = () => {
                    s.style.visibility = "visible"
                }, e.onmouseleave = () => {
                    s.style.visibility = "hidden"
                }, e.appendChild(s), e
            }

            getFormInputLabel(t, e) {
                const s = document.createElement("label");
                return s.appendChild(document.createTextNode(t)), e && s.classList.add("required"), s
            }

            getHeader(t) {
                const e = document.createElement("h3");
                return "string" == typeof t ? e.textContent = t : e.appendChild(t), e.style.display = "inline-block", e
            }

            getCheckbox() {
                const t = this.getFormInputField("checkbox");
                return t.style.display = "inline-block", t.style.width = "auto", t
            }

            getCheckboxLabel(t, e) {
                const s = document.createElement("label");
                return s.appendChild(document.createTextNode(" " + t)), e && s.classList.add("required"), s
            }

            getMultiCheckboxHolder(t, e, s, i) {
                const r = document.createElement("div");
                return r.classList.add("control-group"), e && (e.style.display = "block", r.appendChild(e), i && e.appendChild(i)), Object.values(t).forEach(t => {
                    t.style.display = "inline-block", t.style.marginRight = "20px", r.appendChild(t)
                }), s && r.appendChild(s), r
            }

            getFormCheckboxControl(t, e, s) {
                const i = document.createElement("div");
                return i.appendChild(t), e.style.width = "auto", t.insertBefore(e, t.firstChild), s && this.applyStyles(i, {
                    display: "inline-block",
                    marginRight: "1rem"
                }), i
            }

            getFormRadio(t) {
                const e = this.getFormInputField("radio");
                for (const s in t) e.setAttribute(s, t[s]);
                return e.style.display = "inline-block", e.style.width = "auto", e
            }

            getFormRadioLabel(t, e) {
                const s = document.createElement("label");
                return s.appendChild(document.createTextNode(" " + t)), e && s.classList.add("required"), s
            }

            getFormRadioControl(t, e, s) {
                const i = document.createElement("div");
                return i.appendChild(t), e.style.width = "auto", t.insertBefore(e, t.firstChild), s && this.applyStyles(i, {
                    display: "inline-block",
                    marginRight: "1rem"
                }), i
            }

            getSelectInput(t, e) {
                const s = document.createElement("select");
                return t && this.setSelectOptions(s, t), s
            }

            getSwitcher(t) {
                const e = this.getSelectInput(t, !1);
                return e.style.backgroundColor = "transparent", e.style.display = "inline-block", e.style.fontStyle = "italic", e.style.fontWeight = "normal", e.style.height = "auto", e.style.marginBottom = 0, e.style.marginLeft = "5px", e.style.padding = "0 0 0 3px", e.style.width = "auto", e
            }

            getSwitcherOptions(t) {
                return t.getElementsByTagName("option")
            }

            setSwitcherOptions(t, e, s) {
                this.setSelectOptions(t, e, s)
            }

            setSelectOptions(t, e, s = []) {
                t.innerHTML = "";
                for (let i = 0; i < e.length; i++) {
                    const r = document.createElement("option");
                    r.setAttribute("value", e[i]), r.textContent = s[i] || e[i], t.appendChild(r)
                }
            }

            getTextareaInput() {
                const t = document.createElement("textarea");
                return t.style = t.style || {}, t.style.width = "100%", t.style.height = "300px", t.style.boxSizing = "border-box", t
            }

            getRangeInput(t, e, s) {
                const i = this.getFormInputField("range");
                return i.setAttribute("min", t), i.setAttribute("max", e), i.setAttribute("step", s), i
            }

            getRangeOutput(t, e) {
                const s = document.createElement("output");
                s.value = e || 0;
                const i = t => {
                    s.value = t.currentTarget.value
                };
                return t.addEventListener("change", i, !1), t.addEventListener("input", i, !1), s
            }

            getRangeControl(t, e) {
                const s = document.createElement("div");
                return s.style.textAlign = "center", e && s.appendChild(e), s.appendChild(t), s
            }

            getFormInputField(t) {
                const e = document.createElement("input");
                return e.setAttribute("type", t), e
            }

            afterInputReady(t) {
            }

            getFormControl(t, e, s, i) {
                const r = document.createElement("div");
                return r.classList.add("form-control"), t && r.appendChild(t), "checkbox" !== e.type && "radio" !== e.type || !t ? (i && t && t.appendChild(i), r.appendChild(e)) : (e.style.width = "auto", t.insertBefore(e, t.firstChild), i && t.appendChild(i)), s && r.appendChild(s), r
            }

            getIndentedPanel() {
                const t = document.createElement("div");
                return t.style = t.style || {}, t.style.paddingLeft = "10px", t.style.marginLeft = "10px", t.style.borderLeft = "1px solid #ccc", t
            }

            getTopIndentedPanel() {
                const t = document.createElement("div");
                return t.style = t.style || {}, t.style.paddingLeft = "10px", t.style.marginLeft = "10px", t
            }

            getChildEditorHolder() {
                return document.createElement("div")
            }

            getDescription(t) {
                const e = document.createElement("p");
                return window.DOMPurify ? e.innerHTML = window.DOMPurify.sanitize(t) : e.textContent = this.cleanText(t), e
            }

            getCheckboxDescription(t) {
                return this.getDescription(t)
            }

            getFormInputDescription(t) {
                return this.getDescription(t)
            }

            getButtonHolder() {
                return document.createElement("span")
            }

            getHeaderButtonHolder() {
                return this.getButtonHolder()
            }

            getFormButtonHolder(t) {
                return this.getButtonHolder()
            }

            getButton(t, e, s) {
                const i = document.createElement("button");
                return i.type = "button", this.setButtonText(i, t, e, s), i
            }

            getFormButton(t, e, s) {
                return this.getButton(t, e, s)
            }

            setButtonText(t, e, s, i) {
                for (; t.firstChild;) t.removeChild(t.firstChild);
                if (s && (t.appendChild(s), e = " " + e), !this.jsoneditor.options.iconlib || !this.jsoneditor.options.remove_button_labels || !s) {
                    const s = document.createElement("span");
                    s.appendChild(document.createTextNode(e)), t.appendChild(s)
                }
                i && t.setAttribute("title", i)
            }

            getTable() {
                return document.createElement("table")
            }

            getTableRow() {
                return document.createElement("tr")
            }

            getTableHead() {
                return document.createElement("thead")
            }

            getTableBody() {
                return document.createElement("tbody")
            }

            getTableHeaderCell(t) {
                const e = document.createElement("th");
                return e.textContent = t, e
            }

            getTableCell() {
                return document.createElement("td")
            }

            getErrorMessage(t) {
                const e = document.createElement("p");
                return e.style = e.style || {}, e.style.color = "red", e.appendChild(document.createTextNode(t)), e
            }

            addInputError(t, e) {
            }

            removeInputError(t) {
            }

            addTableRowError(t) {
            }

            removeTableRowError(t) {
            }

            getTabHolder(t) {
                const e = void 0 === t ? "" : t, s = document.createElement("div");
                return s.innerHTML = `<div style='float: left; width: 130px;' class='tabs'></div><div class='content' style='margin-left: 120px;' id='${e}'></div><div style='clear:both;'></div>`, s
            }

            getTopTabHolder(t) {
                const e = void 0 === t ? "" : t, s = document.createElement("div");
                return s.innerHTML = `<div class='tabs' style='margin-left: 10px;'></div><div style='clear:both;'></div><div class='content' id='${e}'></div>`, s
            }

            applyStyles(t, e) {
                Object.keys(e).forEach(s => t.style[s] = e[s])
            }

            closest(t, e) {
                for (; t && t !== document;) {
                    if (!t[R]) return !1;
                    if (t[R](e)) return t;
                    t = t.parentNode
                }
                return !1
            }

            insertBasicTopTab(t, e) {
                e.firstChild.insertBefore(t, e.firstChild.firstChild)
            }

            getTab(t, e) {
                const s = document.createElement("div");
                return s.appendChild(t), s.id = e, s.style = s.style || {}, this.applyStyles(s, {
                    border: "1px solid #ccc",
                    borderWidth: "1px 0 1px 1px",
                    textAlign: "center",
                    lineHeight: "30px",
                    borderRadius: "5px",
                    borderBottomRightRadius: 0,
                    borderTopRightRadius: 0,
                    fontWeight: "bold",
                    cursor: "pointer"
                }), s
            }

            getTopTab(t, e) {
                const s = document.createElement("div");
                return s.id = e, s.appendChild(t), s.style = s.style || {}, this.applyStyles(s, {
                    float: "left",
                    border: "1px solid #ccc",
                    borderWidth: "1px 1px 0px 1px",
                    textAlign: "center",
                    lineHeight: "30px",
                    borderRadius: "5px",
                    paddingLeft: "5px",
                    paddingRight: "5px",
                    borderBottomRightRadius: 0,
                    borderBottomLeftRadius: 0,
                    fontWeight: "bold",
                    cursor: "pointer"
                }), s
            }

            getTabContentHolder(t) {
                return t.children[1]
            }

            getTopTabContentHolder(t) {
                return t.children[1]
            }

            getTabContent() {
                return this.getIndentedPanel()
            }

            getTopTabContent() {
                return this.getTopIndentedPanel()
            }

            markTabActive(t) {
                this.applyStyles(t.tab, {
                    opacity: 1,
                    background: "white"
                }), void 0 !== t.rowPane ? t.rowPane.style.display = "" : t.container.style.display = ""
            }

            markTabInactive(t) {
                this.applyStyles(t.tab, {
                    opacity: .5,
                    background: ""
                }), void 0 !== t.rowPane ? t.rowPane.style.display = "none" : t.container.style.display = "none"
            }

            addTab(t, e) {
                t.children[0].appendChild(e)
            }

            addTopTab(t, e) {
                t.children[0].appendChild(e)
            }

            getBlockLink() {
                const t = document.createElement("a");
                return t.style.display = "block", t
            }

            getBlockLinkHolder() {
                return document.createElement("div")
            }

            getLinksHolder() {
                return document.createElement("div")
            }

            createMediaLink(t, e, s) {
                t.appendChild(e), s.style.width = "100%", t.appendChild(s)
            }

            createImageLink(t, e, s) {
                t.appendChild(e), e.appendChild(s)
            }

            getFirstTab(t) {
                return t.firstChild.firstChild
            }

            getInputGroup(t, e) {
            }

            cleanText(t) {
                const e = document.createElement("div");
                return e.innerHTML = t, e.textContent || e.innerText
            }

            getDropZone(t) {
                const e = document.createElement("div");
                return e.setAttribute("data-text", t), e.classList.add("je-dropzone"), e
            }

            getUploadPreview(t, e, s) {
                const i = document.createElement("div");
                if (i.classList.add("je-upload-preview"), "image" === t.mimeType.substr(0, 5)) {
                    const t = document.createElement("img");
                    t.src = s, i.appendChild(t)
                }
                const r = document.createElement("div");
                r.innerHTML += `<strong>Name:</strong> ${t.name}<br><strong>Type:</strong> ${t.type}<br><strong>Size:</strong> ${t.formattedSize}`, i.appendChild(r), i.appendChild(e);
                const o = document.createElement("div");
                return o.style.clear = "left", i.appendChild(o), i
            }

            getProgressBar() {
                const t = document.createElement("progress");
                return t.setAttribute("max", 100), t.setAttribute("value", 0), t
            }

            updateProgressBar(t, e) {
                t && t.setAttribute("value", e)
            }

            updateProgressBarUnknown(t) {
                t && t.removeAttribute("value")
            }
        }

        P.rules = {".je-upload-preview img": "float:left;margin:0 0.5rem 0.5rem 0;max-width:100%;max-height:100px"};

        class z extends P {
            getFormInputLabel(t, e) {
                const s = super.getFormInputLabel(t, e);
                return s.classList.add("je-form-input-label"), s
            }

            getFormInputDescription(t) {
                const e = super.getFormInputDescription(t);
                return e.classList.add("je-form-input-label"), e
            }

            getIndentedPanel() {
                const t = super.getIndentedPanel();
                return t.classList.add("je-indented-panel"), t
            }

            getTopIndentedPanel() {
                return this.getIndentedPanel()
            }

            getChildEditorHolder() {
                const t = super.getChildEditorHolder();
                return t.classList.add("je-child-editor-holder"), t
            }

            getHeaderButtonHolder() {
                const t = this.getButtonHolder();
                return t.classList.add("je-header-button-holder"), t
            }

            getTable() {
                const t = super.getTable();
                return t.classList.add("je-table"), t
            }

            addInputError(t, e) {
                if (t.style.borderColor = "red", t.errmsg) t.errmsg.style.display = "block"; else {
                    const e = this.closest(t, ".form-control");
                    t.errmsg = document.createElement("div"), t.errmsg.setAttribute("class", "errmsg"), t.errmsg.style = t.errmsg.style || {}, t.errmsg.style.color = "red", e.appendChild(t.errmsg)
                }
                t.errmsg.innerHTML = "", t.errmsg.appendChild(document.createTextNode(e))
            }

            removeInputError(t) {
                t.style && (t.style.borderColor = ""), t.errmsg && (t.errmsg.style.display = "none")
            }
        }

        z.rules = {
            "je-form-input-label": "display:block;margin-bottom:3px;font-weight:bold",
            "je-form-input-description": "display:inline-block;margin:0;font-size:0.8em;font-style:italic",
            "je-indented-panel": "padding:5px;margin:10px;border-radius:3px;border:1px%20solid%20%23ddd",
            "je-child-editor-holder": "margin-bottom:8px",
            "je-header-button-holder": "display:inline-block;margin-left:10px;font-size:0.8em;vertical-align:middle",
            "je-table": "margin-bottom:5px;border-bottom:1px%20solid%20%23ccc",
            ".je-upload-preview img": "float:left;margin:0%200.5rem%200.5rem%200;max-width:100%25;max-height:5rem",
            ".je-dropzone": "position:relative;margin:0.5rem%200;border:2px%20dashed%20black;width:100%25;height:60px;background:teal;transition:all%200.5s",
            ".je-dropzone:before": "position:absolute;content:attr(data-text);color:rgba(0%2C%200%2C%200%2C%200.6);left:50%25;top:50%25;transform:translate(-50%25%2C%20-50%25)",
            ".je-dropzone.valid-dropzone": "background:green",
            ".je-dropzone.invalid-dropzone": "background:red"
        };
        var M = s(0), D = s.n(M);

        class q extends P {
            getSelectInput(t, e) {
                const s = super.getSelectInput(t);
                return s.classList.add("form-control"), s
            }

            setGridColumnSize(t, e, s) {
                t.classList.add("col-md-" + e), s && t.classList.add("col-md-offset-" + s)
            }

            afterInputReady(t) {
                if (!t.controlgroup && (t.controlgroup = this.closest(t, ".form-group"), this.closest(t, ".compact") && (t.controlgroup.style.marginBottom = 0), this.queuedInputErrorText)) {
                    const e = this.queuedInputErrorText;
                    delete this.queuedInputErrorText, this.addInputError(t, e)
                }
            }

            getTextareaInput() {
                const t = document.createElement("textarea");
                return t.classList.add("form-control"), t
            }

            getRangeInput(t, e, s) {
                return super.getRangeInput(t, e, s)
            }

            getFormInputField(t) {
                const e = super.getFormInputField(t);
                return "checkbox" !== t && "radio" !== t && e.classList.add("form-control"), e
            }

            getFormControl(t, e, s) {
                const i = document.createElement("div");
                return !t || "checkbox" !== e.type && "radio" !== e.type ? (i.classList.add("form-group"), t && (t.classList.add("control-label"), i.appendChild(t)), i.appendChild(e)) : (i.classList.add(e.type), t.insertBefore(e, t.firstChild), i.appendChild(t)), s && i.appendChild(s), i
            }

            getIndentedPanel() {
                const t = document.createElement("div");
                return t.classList.add("well", "well-sm"), t.style.paddingBottom = 0, t
            }

            getInfoButton(t) {
                const e = document.createElement("span");
                e.classList.add("glyphicon", "glyphicon-info-sign", "pull-right"), e.style.padding = ".25rem", e.style.position = "relative", e.style.display = "inline-block";
                const s = document.createElement("span");
                return s.style["font-family"] = "sans-serif", s.style.visibility = "hidden", s.style["background-color"] = "rgba(50, 50, 50, .75)", s.style.margin = "0 .25rem", s.style.color = "#FAFAFA", s.style.padding = ".5rem 1rem", s.style["border-radius"] = ".25rem", s.style.width = "25rem", s.style.transform = "translateX(-27rem) translateY(-.5rem)", s.style.position = "absolute", s.innerText = t, e.onmouseover = () => {
                    s.style.visibility = "visible"
                }, e.onmouseleave = () => {
                    s.style.visibility = "hidden"
                }, e.appendChild(s), e
            }

            getFormInputDescription(t) {
                const e = document.createElement("p");
                return e.classList.add("help-block"), window.DOMPurify ? e.innerHTML = window.DOMPurify.sanitize(t) : e.textContent = this.cleanText(t), e
            }

            getHeaderButtonHolder() {
                const t = this.getButtonHolder();
                return t.style.marginLeft = "10px", t
            }

            getButtonHolder() {
                const t = document.createElement("span");
                return t.classList.add("btn-group"), t
            }

            getButton(t, e, s) {
                const i = super.getButton(t, e, s);
                return i.classList.add("btn", "btn-default"), i
            }

            getTable() {
                const t = document.createElement("table");
                return t.classList.add("table", "table-bordered"), t.style.width = "auto", t.style.maxWidth = "none", t
            }

            addInputError(t, e) {
                t.controlgroup ? (t.controlgroup.classList.add("has-error"), t.errmsg ? t.errmsg.style.display = "" : (t.errmsg = document.createElement("p"), t.errmsg.classList.add("help-block", "errormsg"), t.controlgroup.appendChild(t.errmsg)), t.errmsg.textContent = e) : this.queuedInputErrorText = e
            }

            removeInputError(t) {
                t.controlgroup || delete this.queuedInputErrorText, t.errmsg && (t.errmsg.style.display = "none", t.controlgroup.classList.remove("has-error"))
            }

            getTabHolder(t) {
                const e = void 0 === t ? "" : t, s = document.createElement("div");
                return s.innerHTML = `<ul class='col-md-2 nav nav-pills nav-stacked' id='${e}' role='tablist'></ul><div class='col-md-10 tab-content well well-small'  id='${e}'></div>`, s
            }

            getTopTabHolder(t) {
                const e = void 0 === t ? "" : t, s = document.createElement("div");
                return s.innerHTML = `<ul class='nav nav-tabs' id='${e}' role='tablist'></ul><div class='tab-content well well-small'  id='${e}'></div>`, s
            }

            getTab(t, e) {
                const s = document.createElement("li");
                s.setAttribute("role", "presentation");
                const i = document.createElement("a");
                return i.setAttribute("href", "#" + e), i.appendChild(t), i.setAttribute("aria-controls", e), i.setAttribute("role", "tab"), i.setAttribute("data-toggle", "tab"), s.appendChild(i), s
            }

            getTopTab(t, e) {
                const s = document.createElement("li");
                s.setAttribute("role", "presentation");
                const i = document.createElement("a");
                return i.setAttribute("href", "#" + e), i.appendChild(t), i.setAttribute("aria-controls", e), i.setAttribute("role", "tab"), i.setAttribute("data-toggle", "tab"), s.appendChild(i), s
            }

            getTabContent() {
                const t = document.createElement("div");
                return t.classList.add("tab-pane"), t.setAttribute("role", "tabpanel"), t
            }

            getTopTabContent() {
                const t = document.createElement("div");
                return t.classList.add("tab-pane"), t.setAttribute("role", "tabpanel"), t
            }

            markTabActive(t) {
                t.tab.classList.add("active"), void 0 !== t.rowPane ? t.rowPane.classList.add("active") : t.container.classList.add("active")
            }

            markTabInactive(t) {
                t.tab.classList.remove("active"), void 0 !== t.rowPane ? t.rowPane.classList.remove("active") : t.container.classList.remove("active")
            }

            getProgressBar() {
                const t = document.createElement("div");
                t.classList.add("progress");
                const e = document.createElement("div");
                return e.classList.add("progress-bar"), e.setAttribute("role", "progressbar"), e.setAttribute("aria-valuenow", 0), e.setAttribute("aria-valuemin", 0), e.setAttribute("aria-valuenax", 100), e.innerHTML = "0%", t.appendChild(e), t
            }

            updateProgressBar(t, e) {
                if (!t) return;
                const s = t.firstChild, i = e + "%";
                s.setAttribute("aria-valuenow", e), s.style.width = i, s.innerHTML = i
            }

            updateProgressBarUnknown(t) {
                if (!t) return;
                const e = t.firstChild;
                t.classList.add("progress", "progress-striped", "active"), e.removeAttribute("aria-valuenow"), e.style.width = "100%", e.innerHTML = ""
            }

            getInputGroup(t, e) {
                if (!t) return;
                const s = document.createElement("div");
                s.classList.add("input-group"), s.appendChild(t);
                const i = document.createElement("div");
                i.classList.add("input-group-btn"), s.appendChild(i);
                for (let t = 0; t < e.length; t++) i.appendChild(e[t]);
                return s
            }
        }

        q.rules = D.a;
        const $ = {
            disable_theme_rules: !1,
            input_size: "normal",
            custom_forms: !1,
            object_indent: !0,
            object_background: "bg-light",
            object_text: "",
            table_border: !1,
            table_zebrastyle: !1,
            tooltip: "bootstrap"
        };

        class G extends P {
            constructor(t) {
                super(t, $)
            }

            getSelectInput(t, e) {
                const s = super.getSelectInput(t);
                return s.classList.add("form-control"), !1 === this.options.custom_forms ? ("small" === this.options.input_size && s.classList.add("form-control-sm"), "large" === this.options.input_size && s.classList.add("form-control-lg")) : (s.classList.remove("form-control"), s.classList.add("custom-select"), "small" === this.options.input_size && s.classList.add("custom-select-sm"), "large" === this.options.input_size && s.classList.add("custom-select-lg")), s
            }

            getContainer() {
                const t = document.createElement("div");
                return this.options.object_indent || t.classList.add("je-noindent"), t
            }

            setGridColumnSize(t, e, s) {
                t.classList.add("col-md-" + e), s && t.classList.add("offset-md-" + s)
            }

            afterInputReady(t) {
                if (t.controlgroup) return;
                const e = t.name;
                t.id = e;
                const s = t.parentNode.parentNode.getElementsByTagName("label")[0];
                s && (s.htmlFor = e), t.controlgroup = this.closest(t, ".form-group")
            }

            getTextareaInput() {
                const t = document.createElement("textarea");
                return t.classList.add("form-control"), "small" === this.options.input_size && t.classList.add("form-control-sm"), "large" === this.options.input_size && t.classList.add("form-control-lg"), t
            }

            getRangeInput(t, e, s) {
                const i = super.getRangeInput(t, e, s);
                return !0 === this.options.custom_forms && (i.classList.remove("form-control"), i.classList.add("custom-range")), i
            }

            getFormInputField(t) {
                const e = super.getFormInputField(t);
                return "checkbox" !== t && "radio" !== t && "file" !== t && (e.classList.add("form-control"), "small" === this.options.input_size && e.classList.add("form-control-sm"), "large" === this.options.input_size && e.classList.add("form-control-lg")), "file" === t && e.classList.add("form-control-file"), e
            }

            getFormControl(t, e, s, i) {
                const r = document.createElement("div");
                if (r.classList.add("form-group"), !t || "checkbox" !== e.type && "radio" !== e.type) t && (r.appendChild(t), i && r.appendChild(i)), r.appendChild(e); else {
                    const s = document.createElement("div");
                    !1 === this.options.custom_forms ? (s.classList.add("form-check"), e.classList.add("form-check-input"), t.classList.add("form-check-label")) : (s.classList.add("custom-control"), e.classList.add("custom-control-input"), t.classList.add("custom-control-label"), "checkbox" === e.type ? s.classList.add("custom-checkbox") : s.classList.add("custom-radio"));
                    const o = (Date.now() * Math.random()).toFixed(0);
                    e.setAttribute("id", o), t.setAttribute("for", o), s.appendChild(e), s.appendChild(t), i && s.appendChild(i), r.appendChild(s)
                }
                return s && r.appendChild(s), r
            }

            getInfoButton(t) {
                const e = document.createElement("button");
                e.type = "button", e.classList.add("ml-3", "jsoneditor-twbs4-text-button"), e.setAttribute("data-toggle", "tooltip"), e.setAttribute("data-placement", "auto"), e.title = t;
                const s = document.createTextNode("ⓘ");
                return e.appendChild(s), "bootstrap" === this.options.tooltip ? window.jQuery && window.jQuery().tooltip ? window.jQuery(e).tooltip() : console.warn("Could not find popper jQuery plugin of Bootstrap.") : "css" === this.options.tooltip && e.classList.add("je-tooltip"), e
            }

            getCheckbox() {
                return this.getFormInputField("checkbox")
            }

            getMultiCheckboxHolder(t, e, s, i) {
                const r = document.createElement("div");
                r.classList.add("form-group"), e && (r.appendChild(e), i && e.appendChild(i));
                const o = document.createElement("div");
                return Object.values(t).forEach(t => {
                    const e = t.firstChild;
                    o.appendChild(e)
                }), r.appendChild(o), s && r.appendChild(s), r
            }

            getFormRadio(t) {
                const e = this.getFormInputField("radio");
                for (const s in t) e.setAttribute(s, t[s]);
                return !1 === this.options.custom_forms ? e.classList.add("form-check-input") : e.classList.add("custom-control-input"), e
            }

            getFormRadioLabel(t, e) {
                const s = document.createElement("label");
                return !1 === this.options.custom_forms ? s.classList.add("form-check-label") : s.classList.add("custom-control-label"), s.appendChild(document.createTextNode(t)), s
            }

            getFormRadioControl(t, e, s) {
                const i = document.createElement("div");
                return !1 === this.options.custom_forms ? i.classList.add("form-check") : i.classList.add("custom-control", "custom-radio"), i.appendChild(e), i.appendChild(t), s && (!1 === this.options.custom_forms ? i.classList.add("form-check-inline") : i.classList.add("custom-control-inline")), i
            }

            getIndentedPanel() {
                const t = document.createElement("div");
                return t.classList.add("card", "card-body", "mb-3"), this.options.object_background && t.classList.add(this.options.object_background), this.options.object_text && t.classList.add(this.options.object_text), t
            }

            getFormInputDescription(t) {
                const e = document.createElement("small");
                return e.classList.add("form-text"), window.DOMPurify ? e.innerHTML = window.DOMPurify.sanitize(t) : e.textContent = this.cleanText(t), e
            }

            getHeader(t) {
                const e = document.createElement("h3");
                return e.classList.add("card-title"), "string" == typeof t ? e.textContent = t : e.appendChild(t), e.style.display = "inline-block", e
            }

            getHeaderButtonHolder() {
                return this.getButtonHolder()
            }

            getButtonHolder() {
                const t = document.createElement("span");
                return t.classList.add("btn-group"), t
            }

            getFormButtonHolder(t) {
                const e = this.getButtonHolder();
                return e.classList.add("d-block"), "center" === t ? e.classList.add("text-center") : "right" === t && e.classList.add("text-right"), e
            }

            getButton(t, e, s) {
                const i = super.getButton(t, e, s);
                return i.classList.add("btn", "btn-secondary", "btn-sm"), i
            }

            getTable() {
                const t = document.createElement("table");
                return t.classList.add("table", "table-sm"), this.options.table_border && t.classList.add("table-bordered"), this.options.table_zebrastyle && t.classList.add("table-striped"), t
            }

            getErrorMessage(t) {
                const e = document.createElement("div");
                return e.classList.add("alert", "alert-danger"), e.setAttribute("role", "alert"), e.appendChild(document.createTextNode(t)), e
            }

            addInputError(t, e) {
                t.controlgroup && (t.classList.add("is-invalid"), t.errmsg ? t.errmsg.style.display = "" : (t.errmsg = document.createElement("p"), t.errmsg.classList.add("invalid-feedback"), t.controlgroup.appendChild(t.errmsg)), t.errmsg.textContent = e)
            }

            removeInputError(t) {
                t.errmsg && (t.errmsg.style.display = "none", t.classList.remove("is-invalid"))
            }

            getTabHolder(t) {
                const e = document.createElement("div"), s = void 0 === t ? "" : t;
                return e.innerHTML = `<div class='col-md-2' id='${s}'><ul class='nav flex-column nav-pills'></ul></div><div class='col-md-10'><div class='tab-content' id='${s}'></div></div>`, e.classList.add("row"), e
            }

            addTab(t, e) {
                t.children[0].children[0].appendChild(e)
            }

            getTabContentHolder(t) {
                return t.children[1].children[0]
            }

            getTopTabHolder(t) {
                const e = void 0 === t ? "" : t, s = document.createElement("div");
                return s.classList.add("card"), s.innerHTML = `<div class='card-header'><ul class='nav nav-tabs card-header-tabs' id='${e}'></ul></div><div class='card-body'><div class='tab-content' id='${e}'></div></div>`, s
            }

            getTab(t, e) {
                const s = document.createElement("li");
                s.classList.add("nav-item");
                const i = document.createElement("a");
                return i.classList.add("nav-link"), i.setAttribute("href", "#" + e), i.setAttribute("data-toggle", "tab"), i.appendChild(t), s.appendChild(i), s
            }

            getTopTab(t, e) {
                const s = document.createElement("li");
                s.classList.add("nav-item");
                const i = document.createElement("a");
                return i.classList.add("nav-link"), i.setAttribute("href", "#" + e), i.setAttribute("data-toggle", "tab"), i.appendChild(t), s.appendChild(i), s
            }

            getTabContent() {
                const t = document.createElement("div");
                return t.classList.add("tab-pane"), t.setAttribute("role", "tabpanel"), t
            }

            getTopTabContent() {
                const t = document.createElement("div");
                return t.classList.add("tab-pane"), t.setAttribute("role", "tabpanel"), t
            }

            markTabActive(t) {
                t.tab.firstChild.classList.add("active"), void 0 !== t.rowPane ? t.rowPane.classList.add("active") : t.container.classList.add("active")
            }

            markTabInactive(t) {
                t.tab.firstChild.classList.remove("active"), void 0 !== t.rowPane ? t.rowPane.classList.remove("active") : t.container.classList.remove("active")
            }

            insertBasicTopTab(t, e) {
                e.children[0].children[0].insertBefore(t, e.children[0].children[0].firstChild)
            }

            addTopTab(t, e) {
                t.children[0].children[0].appendChild(e)
            }

            getTopTabContentHolder(t) {
                return t.children[1].children[0]
            }

            getFirstTab(t) {
                return t.firstChild.firstChild.firstChild
            }

            getProgressBar() {
                const t = document.createElement("div");
                t.classList.add("progress");
                const e = document.createElement("div");
                return e.classList.add("progress-bar"), e.setAttribute("role", "progressbar"), e.setAttribute("aria-valuenow", 0), e.setAttribute("aria-valuemin", 0), e.setAttribute("aria-valuenax", 100), e.innerHTML = "0%", t.appendChild(e), t
            }

            updateProgressBar(t, e) {
                if (!t) return;
                const s = t.firstChild, i = e + "%";
                s.setAttribute("aria-valuenow", e), s.style.width = i, s.innerHTML = i
            }

            updateProgressBarUnknown(t) {
                if (!t) return;
                const e = t.firstChild;
                t.classList.add("progress", "progress-striped", "active"), e.removeAttribute("aria-valuenow"), e.style.width = "100%", e.innerHTML = ""
            }

            getBlockLink() {
                const t = document.createElement("a");
                return t.classList.add("mb-3", "d-inline-block"), t
            }

            getLinksHolder() {
                return document.createElement("div")
            }

            getInputGroup(t, e) {
                if (!t) return;
                const s = document.createElement("div");
                s.classList.add("input-group"), s.appendChild(t);
                const i = document.createElement("div");
                i.classList.add("input-group-append"), s.appendChild(i);
                for (let t = 0; t < e.length; t++) e[t].classList.remove("mr-2", "btn-secondary"), e[t].classList.add("btn-outline-secondary"), i.appendChild(e[t]);
                return s
            }
        }

        G.rules = {
            ".jsoneditor-twbs4-text-button": "background:none;padding:0;border:0;color:currentColor",
            "td > .form-group": "margin-bottom:0",
            ".json-editor-btn-upload": "margin-top:1rem",
            ".je-noindent .card": "padding:0;border:0",
            ".je-tooltip:hover::before": "display:block;position:absolute;font-size:0.8em;color:%23fff;border-radius:0.2em;content:attr(title);background-color:%23000;margin-top:-2.5em;padding:0.3em",
            ".je-tooltip:hover::after": "display:block;position:absolute;font-size:0.8em;color:%23fff",
            ".select2-container--default .select2-selection--single": "height:calc(1.5em%20%2B%200.75rem%20%2B%202px)",
            ".select2-container--default   .select2-selection--single   .select2-selection__arrow": "height:calc(1.5em%20%2B%200.75rem%20%2B%202px)",
            ".select2-container--default   .select2-selection--single   .select2-selection__rendered": "line-height:calc(1.5em%20%2B%200.75rem%20%2B%202px)",
            ".selectize-control.form-control": "padding:0",
            ".selectize-dropdown.form-control": "padding:0;height:auto",
            ".je-upload-preview img": "float:left;margin:0%200.5rem%200.5rem%200;max-width:100%25;max-height:5rem",
            ".je-dropzone": "position:relative;margin:0.5rem%200;border:2px%20dashed%20black;width:100%25;height:60px;background:teal;transition:all%200.5s",
            ".je-dropzone:before": "position:absolute;content:attr(data-text);color:rgba(0%2C%200%2C%200%2C%200.6);left:50%25;top:50%25;transform:translate(-50%25%2C%20-50%25)",
            ".je-dropzone.valid-dropzone": "background:green",
            ".je-dropzone.invalid-dropzone": "background:red"
        };

        class U extends P {
            getTable() {
                const t = super.getTable();
                return t.setAttribute("cellpadding", 5), t.setAttribute("cellspacing", 0), t
            }

            getTableHeaderCell(t) {
                const e = super.getTableHeaderCell(t);
                return e.classList.add("ui-state-active"), e.style.fontWeight = "bold", e
            }

            getTableCell() {
                const t = super.getTableCell();
                return t.classList.add("ui-widget-content"), t
            }

            getHeaderButtonHolder() {
                const t = this.getButtonHolder();
                return t.style.marginLeft = "10px", t.style.fontSize = ".6em", t.style.display = "inline-block", t
            }

            getFormInputDescription(t) {
                const e = this.getDescription(t);
                return e.style.marginLeft = "10px", e.style.display = "inline-block", e
            }

            getFormControl(t, e, s, i) {
                const r = super.getFormControl(t, e, s, i);
                return "checkbox" === e.type ? (r.style.lineHeight = "25px", r.style.padding = "3px 0") : r.style.padding = "4px 0 8px 0", r
            }

            getDescription(t) {
                const e = document.createElement("span");
                return e.style.fontSize = ".8em", e.style.fontStyle = "italic", window.DOMPurify ? e.innerHTML = window.DOMPurify.sanitize(t) : e.textContent = this.cleanText(t), e
            }

            getButtonHolder() {
                const t = document.createElement("div");
                return t.classList.add("ui-buttonset"), t.style.fontSize = ".7em", t
            }

            getFormInputLabel(t, e) {
                const s = document.createElement("label");
                return s.style.fontWeight = "bold", s.style.display = "block", s.textContent = t, e && s.classList.add("required"), s
            }

            getButton(t, e, s) {
                const i = document.createElement("button");
                i.classList.add("ui-button", "ui-widget", "ui-state-default", "ui-corner-all"), e && !t ? (i.classList.add("ui-button-icon-only"), e.classList.add("ui-button-icon-primary", "ui-icon-primary"), i.appendChild(e)) : e ? (i.classList.add("ui-button-text-icon-primary"), e.classList.add("ui-button-icon-primary", "ui-icon-primary"), i.appendChild(e)) : i.classList.add("ui-button-text-only");
                const r = document.createElement("span");
                return r.classList.add("ui-button-text"), r.textContent = t || s || ".", i.appendChild(r), i.setAttribute("title", s), i
            }

            setButtonText(t, e, s, i) {
                t.innerHTML = "", t.classList.add("ui-button", "ui-widget", "ui-state-default", "ui-corner-all"), s && !e ? (t.classList.add("ui-button-icon-only"), s.classList.add("ui-button-icon-primary", "ui-icon-primary"), t.appendChild(s)) : s ? (t.classList.add("ui-button-text-icon-primary"), s.classList.add("ui-button-icon-primary", "ui-icon-primary"), t.appendChild(s)) : t.classList.add("ui-button-text-only");
                const r = document.createElement("span");
                r.classList.add("ui-button-text"), r.textContent = e || i || ".", t.appendChild(r), t.setAttribute("title", i)
            }

            getIndentedPanel() {
                const t = document.createElement("div");
                return t.classList.add("ui-widget-content", "ui-corner-all"), t.style.padding = "1em 1.4em", t.style.marginBottom = "20px", t
            }

            afterInputReady(t) {
                if (!t.controls && (t.controls = this.closest(t, ".form-control"), this.queuedInputErrorText)) {
                    const e = this.queuedInputErrorText;
                    delete this.queuedInputErrorText, this.addInputError(t, e)
                }
            }

            addInputError(t, e) {
                t.controls ? (t.errmsg ? t.errmsg.style.display = "" : (t.errmsg = document.createElement("div"), t.errmsg.classList.add("ui-state-error"), t.controls.appendChild(t.errmsg)), t.errmsg.textContent = e) : this.queuedInputErrorText = e
            }

            removeInputError(t) {
                t.controls || delete this.queuedInputErrorText, t.errmsg && (t.errmsg.style.display = "none")
            }

            markTabActive(t) {
                t.tab.classList.remove("ui-widget-header"), t.tab.classList.add("ui-state-active"), void 0 !== t.rowPane ? t.rowPane.style.display = "" : t.container.style.display = ""
            }

            markTabInactive(t) {
                t.tab.classList.add("ui-widget-header"), t.tab.classList.remove("ui-state-active"), void 0 !== t.rowPane ? t.rowPane.style.display = "none" : t.container.style.display = "none"
            }
        }

        U.rules = {'div[data-schemaid="root"]:after': 'position:relative;color:red;margin:10px 0;font-weight:600;display:block;width:100%;text-align:center;content:"This is an old JSON-Editor 1.x Theme and might not display elements correctly when used with the 2.x version"'};

        class J extends P {
            addInputError(t, e) {
                if (t.errmsg) t.errmsg.style.display = "block"; else {
                    const e = this.closest(t, ".form-control");
                    t.errmsg = document.createElement("div"), t.errmsg.setAttribute("class", "errmsg"), e.appendChild(t.errmsg)
                }
                t.errmsg.innerHTML = "", t.errmsg.appendChild(document.createTextNode(e))
            }

            removeInputError(t) {
                t.style && (t.style.borderColor = ""), t.errmsg && (t.errmsg.style.display = "none")
            }
        }

        J.rules = {
            ".je-upload-preview img": "float:left;margin:0%200.5rem%200.5rem%200;max-width:100%25;max-height:5rem",
            ".je-dropzone": "position:relative;margin:0.5rem%200;border:2px%20dashed%20black;width:100%25;height:60px;background:teal;transition:all%200.5s",
            ".je-dropzone:before": "position:absolute;content:attr(data-text);color:rgba(0%2C%200%2C%200%2C%200.6);left:50%25;top:50%25;transform:translate(-50%25%2C%20-50%25)",
            ".je-dropzone.valid-dropzone": "background:green",
            ".je-dropzone.invalid-dropzone": "background:red"
        };
        const W = {
            disable_theme_rules: !1,
            label_bold: !0,
            align_bottom: !1,
            object_indent: !1,
            object_border: !1,
            table_border: !1,
            table_zebrastyle: !1,
            input_size: "normal"
        };

        class Z extends P {
            constructor(t) {
                super(t, W)
            }

            setGridColumnSize(t, e, s) {
                t.classList.add("col-" + e), s && t.classList.add("col-mx-auto")
            }

            getGridContainer() {
                const t = document.createElement("div");
                return t.classList.add("container"), this.options.object_indent || t.classList.add("je-noindent"), t
            }

            getGridRow() {
                const t = document.createElement("div");
                return t.classList.add("columns"), t
            }

            getGridColumn() {
                const t = document.createElement("div");
                return t.classList.add("column"), this.options.align_bottom && t.classList.add("je-align-bottom"), t
            }

            getIndentedPanel() {
                const t = document.createElement("div");
                return t.classList.add("je-panel"), this.options.object_border && t.classList.add("je-border"), t
            }

            getTopIndentedPanel() {
                const t = document.createElement("div");
                return t.classList.add("je-panel-top"), this.options.object_border && t.classList.add("je-border"), t
            }

            getHeaderButtonHolder() {
                return this.getButtonHolder()
            }

            getButtonHolder() {
                const t = super.getButtonHolder();
                return t.classList.add("btn-group"), t
            }

            getFormButtonHolder(t) {
                const e = super.getFormButtonHolder();
                return e.classList.remove("btn-group"), e.classList.add("d-block"), "center" === t ? e.classList.add("text-center") : "right" === t ? e.classList.add("text-right") : e.classList.add("text-left"), e
            }

            getFormButton(t, e, s) {
                const i = super.getFormButton(t, e, s);
                return i.classList.add("btn", "btn-primary", "mx-2", "my-1"), "small" !== this.options.input_size && i.classList.remove("btn-sm"), "large" === this.options.input_size && i.classList.add("btn-lg"), i
            }

            getButton(t, e, s) {
                const i = super.getButton(t, e, s);
                return i.classList.add("btn", "btn-sm", "btn-primary", "mr-2", "my-1"), i
            }

            getHeader(t) {
                const e = document.createElement("h4");
                return "string" == typeof t ? e.textContent = t : e.appendChild(t), e.style.display = "inline-block", e
            }

            getFormInputDescription(t) {
                const e = super.getFormInputDescription(t);
                return e.classList.add("je-desc", "hide-sm"), e
            }

            getFormInputLabel(t, e) {
                const s = super.getFormInputLabel(t, e);
                return this.options.label_bold && s.classList.add("je-label"), s
            }

            getCheckbox() {
                return this.getFormInputField("checkbox")
            }

            getCheckboxLabel(t, e) {
                const s = super.getCheckboxLabel(t, e), i = document.createElement("i");
                return i.classList.add("form-icon"), s.classList.add("form-checkbox", "mr-5"), s.insertBefore(i, s.firstChild), s
            }

            getFormCheckboxControl(t, e, s) {
                return t.insertBefore(e, t.firstChild), s && t.classList.add("form-inline"), t
            }

            getMultiCheckboxHolder(t, e, s, i) {
                return console.log("mul"), super.getMultiCheckboxHolder(t, e, s, i)
            }

            getFormRadio(t) {
                const e = this.getFormInputField("radio");
                for (const s in t) e.setAttribute(s, t[s]);
                return e
            }

            getFormRadioLabel(t, e) {
                const s = super.getFormRadioLabel(t, e), i = document.createElement("i");
                return i.classList.add("form-icon"), s.classList.add("form-radio"), s.insertBefore(i, s.firstChild), s
            }

            getFormRadioControl(t, e, s) {
                return t.insertBefore(e, t.firstChild), s && t.classList.add("form-inline"), t
            }

            getFormInputField(t) {
                const e = super.getFormInputField(t);
                return ["checkbox", "radio"].includes(t) || e.classList.add("form-input"), e
            }

            getRangeInput(t, e, s) {
                const i = this.getFormInputField("range");
                return i.classList.add("slider"), i.classList.remove("form-input"), i.setAttribute("oninput", 'this.setAttribute("value", this.value)'), i.setAttribute("min", t), i.setAttribute("max", e), i.setAttribute("step", s), i
            }

            getRangeControl(t, e) {
                const s = super.getRangeControl(t, e);
                return s.classList.add("text-center"), s
            }

            getSelectInput(t, e) {
                const s = super.getSelectInput(t);
                return s.classList.add("form-select"), s
            }

            getTextareaInput() {
                const t = document.createElement("textarea");
                return t.classList.add("form-input"), t
            }

            getFormControl(t, e, s, i) {
                const r = document.createElement("div");
                return r.classList.add("form-group"), t && ("checkbox" === e.type && (t = this.getFormCheckboxControl(t, e, !1)), t.classList.add("form-label"), r.appendChild(t), i && r.insertBefore(i, r.firstChild)), "small" === this.options.input_size ? e.classList.add("input-sm", "select-sm") : "large" === this.options.input_size && e.classList.add("input-lg", "select-lg"), "checkbox" !== e.type && r.appendChild(e), s && r.appendChild(s), r
            }

            getInputGroup(t, e) {
                if (!t) return;
                const s = document.createElement("div");
                s.classList.add("input-group"), s.appendChild(t);
                for (let t = 0; t < e.length; t++) e[t].classList.add("input-group-btn"), e[t].classList.remove("btn-sm", "mr-2", "my-1"), s.appendChild(e[t]);
                return s
            }

            getInfoButton(t) {
                const e = document.createElement("div");
                e.classList.add("popover", "popover-left", "float-right");
                const s = document.createElement("button");
                s.classList.add("btn", "btn-secondary", "btn-info", "btn-action", "s-circle"), s.setAttribute("tabindex", "-1"), e.appendChild(s);
                const i = document.createTextNode("I");
                s.appendChild(i);
                const r = document.createElement("div");
                r.classList.add("popover-container"), e.appendChild(r);
                const o = document.createElement("div");
                o.classList.add("card"), r.appendChild(o);
                const n = document.createElement("div");
                return n.classList.add("card-body"), n.innerHTML = t, o.appendChild(n), e
            }

            getTable() {
                const t = super.getTable();
                return t.classList.add("table", "table-scroll"), this.options.table_border && t.classList.add("je-table-border"), this.options.table_zebrastyle && t.classList.add("table-striped"), t
            }

            getProgressBar() {
                const t = super.getProgressBar();
                return t.classList.add("progress"), t
            }

            getTabHolder(t) {
                const e = void 0 === t ? "" : t, s = document.createElement("div");
                return s.classList.add("columns"), s.innerHTML = `<div class="column col-2"></div><div class="column col-10 content" id="${e}"></div>`, s
            }

            getTopTabHolder(t) {
                const e = void 0 === t ? "" : t, s = document.createElement("div");
                return s.innerHTML = `<ul class="tab"></ul><div class="content" id="${e}"></div>`, s
            }

            getTab(t, e) {
                const s = document.createElement("a");
                return s.classList.add("btn", "btn-secondary", "btn-block"), s.setAttribute("href", "#" + e), s.appendChild(t), s
            }

            getTopTab(t, e) {
                const s = document.createElement("li");
                s.id = e, s.classList.add("tab-item");
                const i = document.createElement("a");
                return i.setAttribute("href", "#" + e), i.appendChild(t), s.appendChild(i), s
            }

            markTabActive(t) {
                t.tab.classList.add("active"), void 0 !== t.rowPane ? t.rowPane.style.display = "" : t.container.style.display = ""
            }

            markTabInactive(t) {
                t.tab.classList.remove("active"), void 0 !== t.rowPane ? t.rowPane.style.display = "none" : t.container.style.display = "none"
            }

            afterInputReady(t) {
                if ("select" === t.localName) if (t.classList.contains("selectized")) {
                    const e = t.nextSibling;
                    e && (e.classList.remove("form-select"), Array.from(e.querySelectorAll(".form-select")).forEach(t => {
                        t.classList.remove("form-select")
                    }))
                } else if (t.classList.contains("select2-hidden-accessible")) {
                    const e = t.nextSibling;
                    e && e.querySelector(".select2-selection--single") && e.classList.add("form-select")
                }
                t.controlgroup || (t.controlgroup = this.closest(t, ".form-group"), this.closest(t, ".compact") && (t.controlgroup.style.marginBottom = 0))
            }

            addInputError(t, e) {
                t.controlgroup && (t.controlgroup.classList.add("has-error"), t.errmsg || (t.errmsg = document.createElement("p"), t.errmsg.classList.add("form-input-hint"), t.controlgroup.appendChild(t.errmsg)), t.errmsg.classList.remove("d-hide"), t.errmsg.textContent = e)
            }

            removeInputError(t) {
                t.errmsg && (t.errmsg.classList.add("d-hide"), t.controlgroup.classList.remove("has-error"))
            }
        }

        Z.rules = {
            "*": "--primary-color:%235755d9;--gray-color:%23bcc3ce;--light-color:%23fff",
            ".slider:focus": "box-shadow:none",
            "h4 > label + .btn-group": "margin-left:1rem",
            ".text-right > button": "margin-right:0%20!important",
            ".text-left > button": "margin-left:0%20!important",
            ".property-selector": "font-size:0.7rem;font-weight:normal;max-height:260px%20!important;width:395px%20!important",
            ".property-selector .form-checkbox": "margin:0",
            textarea: "width:100%25;min-height:2rem;resize:vertical",
            table: "border-collapse:collapse",
            ".table td": "padding:0.4rem%200.4rem",
            ".mr-5": "margin-right:1rem%20!important",
            "div[data-schematype]:not([data-schematype='object'])": "transition:0.5s",
            "div[data-schematype]:not([data-schematype='object']):hover": "background-color:%23eee",
            ".je-table-border td": "border:0.05rem%20solid%20%23dadee4%20!important",
            ".btn-info": "font-size:0.5rem;font-weight:bold;height:0.8rem;padding:0.15rem%200;line-height:0.8;margin:0.3rem%200%200.3rem%200.1rem",
            ".je-label + select": "min-width:5rem",
            ".je-label": "font-weight:600",
            ".btn-action.btn-info": "width:0.8rem",
            ".je-border": "border:0.05rem%20solid%20%23dadee4",
            ".je-panel": "padding:0.2rem;margin:0.2rem;background-color:rgba(218%2C%20222%2C%20228%2C%200.1)",
            ".je-panel-top": "padding:0.2rem;margin:0.2rem;background-color:rgba(218%2C%20222%2C%20228%2C%200.1)",
            ".required:after": "content:%22%20*%22;color:red;font:inherit",
            ".je-align-bottom": "margin-top:auto",
            ".je-desc": "font-size:smaller;margin:0.2rem%200",
            ".je-upload-preview img": "float:left;margin:0%200.5rem%200.5rem%200;max-width:100%25;max-height:5rem;border:3px%20solid%20white;box-shadow:0px%200px%208px%20rgba(0%2C%200%2C%200%2C%200.3);box-sizing:border-box",
            ".je-dropzone": "position:relative;margin:0.5rem%200;border:2px%20dashed%20black;width:100%25;height:60px;background:teal;transition:all%200.5s",
            ".je-dropzone:before": "position:absolute;content:attr(data-text);color:rgba(0%2C%200%2C%200%2C%200.6);left:50%25;top:50%25;transform:translate(-50%25%2C%20-50%25)",
            ".je-dropzone.valid-dropzone": "background:green",
            ".je-dropzone.invalid-dropzone": "background:red",
            ".columns .container.je-noindent": "padding-left:0;padding-right:0",
            ".selectize-control.multi .item": "background:var(--primary-color)%20!important",
            ".select2-container--default   .select2-selection--single   .select2-selection__arrow": "display:none",
            ".select2-container--default .select2-selection--single": "border:none",
            ".select2-container .select2-selection--single .select2-selection__rendered": "padding:0",
            ".select2-container .select2-search--inline .select2-search__field": "margin-top:0",
            ".select2-container--default.select2-container--focus   .select2-selection--multiple": "border:0.05rem%20solid%20var(--gray-color)",
            ".select2-container--default   .select2-selection--multiple   .select2-selection__choice": "margin:0.4rem%200.2rem%200.2rem%200;padding:2px%205px;background-color:var(--primary-color);color:var(--light-color)",
            ".select2-container--default .select2-search--inline .select2-search__field": "line-height:normal",
            ".choices": "margin-bottom:auto",
            ".choices__list--multiple .choices__item": "border:none;background-color:var(--primary-color);color:var(--light-color)",
            ".choices[data-type*='select-multiple'] .choices__button": "border-left:0.05rem%20solid%20%232826a6",
            ".choices__inner": "font-size:inherit;min-height:20px;padding:4px%207.5px%204px%203.75px",
            ".choices[data-type*='select-one'] .choices__inner": "padding-bottom:4px",
            ".choices__list--dropdown .choices__item": "font-size:inherit"
        };
        const Q = {
            disable_theme_rules: !1,
            label_bold: !1,
            object_panel_default: !0,
            object_indent: !0,
            object_border: !1,
            table_border: !1,
            table_hdiv: !1,
            table_zebrastyle: !1,
            input_size: "small",
            enable_compact: !1
        };

        class Y extends P {
            constructor(t) {
                super(t, Q)
            }

            getGridContainer() {
                const t = document.createElement("div");
                return t.classList.add("flex", "flex-col", "w-full"), this.options.object_indent || t.classList.add("je-noindent"), t
            }

            getGridRow() {
                const t = document.createElement("div");
                return t.classList.add("flex", "flex-wrap", "w-full"), t
            }

            getGridColumn() {
                const t = document.createElement("div");
                return t.classList.add("flex", "flex-col"), t
            }

            setGridColumnSize(t, e, s) {
                e > 0 && e < 12 ? t.classList.add(`w-${e}/12`, "px-1") : t.classList.add("w-full", "px-1"), s && (t.style.marginLeft = 100 / 12 * s + "%")
            }

            getIndentedPanel() {
                const t = document.createElement("div");
                return this.options.object_panel_default ? t.classList.add("w-full", "p-1") : t.classList.add("relative", "flex", "flex-col", "rounded", "break-words", "border", "bg-white", "border-0", "border-blue-400", "p-1", "shadow-md"), this.options.object_border && t.classList.add("je-border"), t
            }

            getTopIndentedPanel() {
                const t = document.createElement("div");
                return this.options.object_panel_default ? t.classList.add("w-full", "m-2") : t.classList.add("relative", "flex", "flex-col", "rounded", "break-words", "border", "bg-white", "border-0", "border-blue-400", "p-1", "shadow-md"), this.options.object_border && t.classList.add("je-border"), t
            }

            getTitle() {
                return this.schema.title
            }

            getSelectInput(t, e) {
                const s = super.getSelectInput(t);
                return e ? s.classList.add("form-multiselect", "block", "py-0", "h-auto", "w-full", "px-1", "text-sm", "text-black", "leading-normal", "bg-white", "border", "border-grey", "rounded") : s.classList.add("form-select", "block", "py-0", "h-6", "w-full", "px-1", "text-sm", "text-black", "leading-normal", "bg-white", "border", "border-grey", "rounded"), this.options.enable_compact && s.classList.add("compact"), s
            }

            afterInputReady(t) {
                t.controlgroup || (t.controlgroup = this.closest(t, ".form-group"), this.closest(t, ".compact") && (t.controlgroup.style.marginBottom = 0))
            }

            getTextareaInput() {
                const t = super.getTextareaInput();
                return t.classList.add("block", "w-full", "px-1", "text-sm", "leading-normal", "bg-white", "text-black", "border", "border-grey", "rounded"), this.options.enable_compact && t.classList.add("compact"), t.style.height = 0, t
            }

            getRangeInput(t, e, s) {
                const i = this.getFormInputField("range");
                return i.classList.add("slider"), this.options.enable_compact && i.classList.add("compact"), i.setAttribute("oninput", 'this.setAttribute("value", this.value)'), i.setAttribute("min", t), i.setAttribute("max", e), i.setAttribute("step", s), i
            }

            getRangeControl(t, e) {
                const s = super.getRangeControl(t, e);
                return s.classList.add("text-center", "text-black"), s
            }

            getCheckbox() {
                const t = this.getFormInputField("checkbox");
                return t.classList.add("form-checkbox", "text-red-600"), t
            }

            getCheckboxLabel(t, e) {
                const s = super.getCheckboxLabel(t, e);
                return s.classList.add("inline-flex", "items-center"), s
            }

            getFormCheckboxControl(t, e, s) {
                return t.insertBefore(e, t.firstChild), s && t.classList.add("inline-flex flex-row"), t
            }

            getMultiCheckboxHolder(t, e, s, i) {
                const r = super.getMultiCheckboxHolder(t, e, s, i);
                return r.classList.add("inline-flex", "flex-col"), r
            }

            getFormRadio(t) {
                const e = this.getFormInputField("radio");
                e.classList.add("form-radio", "text-red-600");
                for (const s in t) e.setAttribute(s, t[s]);
                return e
            }

            getFormRadioLabel(t, e) {
                const s = super.getFormRadioLabel(t, e);
                return s.classList.add("inline-flex", "items-center", "mr-2"), s
            }

            getFormRadioControl(t, e, s) {
                return t.insertBefore(e, t.firstChild), s && t.classList.add("form-radio"), t
            }

            getRadioHolder(t, e, s, i, r) {
                const o = super.getRadioHolder(e, s, i, r);
                return "h" === t.options.layout ? o.classList.add("inline-flex", "flex-row") : o.classList.add("inline-flex", "flex-col"), o
            }

            getFormInputLabel(t, e) {
                const s = super.getFormInputLabel(t, e);
                return this.options.label_bold ? s.classList.add("font-bold") : s.classList.add("required"), s
            }

            getFormInputField(t) {
                const e = super.getFormInputField(t);
                return ["checkbox", "radio"].includes(t) || e.classList.add("block", "w-full", "px-1", "text-black", "text-sm", "leading-normal", "bg-white", "border", "border-grey", "rounded"), this.options.enable_compact && e.classList.add("compact"), e
            }

            getFormInputDescription(t) {
                const e = document.createElement("p");
                return e.classList.add("block", "mt-1", "text-xs"), window.DOMPurify ? e.innerHTML = window.DOMPurify.sanitize(t) : e.textContent = this.cleanText(t), e
            }

            getFormControl(t, e, s, i) {
                const r = document.createElement("div");
                return r.classList.add("form-group", "mb-1", "w-full"), t && (t.classList.add("text-xs"), "checkbox" === e.type && (e.classList.add("form-checkbox", "text-xs", "text-red-600", "mr-1"), t.classList.add("items-center", "flex"), t = this.getFormCheckboxControl(t, e, !1, i)), "radio" === e.type && (e.classList.add("form-radio", "text-red-600", "mr-1"), t.classList.add("items-center", "flex"), t = this.getFormRadioControl(t, e, !1, i)), r.appendChild(t), !["checkbox", "radio"].includes(e.type) && i && r.appendChild(i)), ["checkbox", "radio"].includes(e.type) || ("small" === this.options.input_size ? e.classList.add("text-xs") : "normal" === this.options.input_size ? e.classList.add("text-base") : "large" === this.options.input_size && e.classList.add("text-xl"), r.appendChild(e)), s && r.appendChild(s), r
            }

            getHeaderButtonHolder() {
                const t = this.getButtonHolder();
                return t.classList.add("text-sm"), t
            }

            getButtonHolder() {
                const t = document.createElement("div");
                return t.classList.add("flex", "relative", "inline-flex", "align-middle"), t
            }

            getButton(t, e, s) {
                const i = super.getButton(t, e, s);
                return i.classList.add("inline-block", "align-middle", "text-center", "text-sm", "bg-blue-700", "text-white", "py-1", "pr-1", "m-2", "shadow", "select-none", "whitespace-no-wrap", "rounded"), i
            }

            getInfoButton(t) {
                const e = document.createElement("a");
                e.classList.add("tooltips", "float-right"), e.innerHTML = "ⓘ";
                const s = document.createElement("span");
                return s.innerHTML = t, e.appendChild(s), e
            }

            getTable() {
                const t = super.getTable();
                return this.options.table_border ? t.classList.add("je-table-border") : t.classList.add("table", "border", "p-0"), t
            }

            getTableRow() {
                const t = super.getTableRow();
                return this.options.table_border && t.classList.add("je-table-border"), this.options.table_zebrastyle && t.classList.add("je-table-zebra"), t
            }

            getTableHeaderCell(t) {
                const e = super.getTableHeaderCell(t);
                return this.options.table_border ? e.classList.add("je-table-border") : this.options.table_hdiv ? e.classList.add("je-table-hdiv") : e.classList.add("text-xs", "border", "p-0", "m-0"), e
            }

            getTableCell() {
                const t = super.getTableCell();
                return this.options.table_border ? t.classList.add("je-table-border") : this.options.table_hdiv ? t.classList.add("je-table-hdiv") : t.classList.add("border-0", "p-0", "m-0"), t
            }

            addInputError(t, e) {
                t.controlgroup && (t.controlgroup.classList.add("has-error"), t.classList.add("bg-red-600"), t.errmsg ? t.errmsg.style.display = "" : (t.errmsg = document.createElement("p"), t.errmsg.classList.add("block", "mt-1", "text-xs", "text-red"), t.controlgroup.appendChild(t.errmsg)), t.errmsg.textContent = e)
            }

            removeInputError(t) {
                t.errmsg && (t.errmsg.style.display = "none", t.classList.remove("bg-red-600"), t.controlgroup.classList.remove("has-error"))
            }

            getTabHolder(t) {
                const e = document.createElement("div"), s = void 0 === t ? "" : t;
                return e.innerHTML = `<div class='w-2/12' id='${s}'><ul class='list-reset pl-0 mb-0'></ul></div><div class='w-10/12' id='${s}'></div>`, e.classList.add("flex"), e
            }

            addTab(t, e) {
                t.children[0].children[0].appendChild(e)
            }

            getTopTabHolder(t) {
                const e = void 0 === t ? "" : t, s = document.createElement("div");
                return s.innerHTML = `<ul class='nav-tabs flex list-reset pl-0 mb-0 border-b border-grey-light' id='${e}'></ul><div class='p-6 block' id='${e}'></div>`, s
            }

            getTab(t, e) {
                const s = document.createElement("li");
                s.classList.add("nav-item", "flex-col", "text-center", "text-white", "bg-blue-500", "shadow-md", "border", "p-2", "mb-2", "mr-2", "hover:bg-blue-400", "rounded");
                const i = document.createElement("a");
                return i.classList.add("nav-link", "text-center"), i.setAttribute("href", "#" + e), i.setAttribute("data-toggle", "tab"), i.appendChild(t), s.appendChild(i), s
            }

            getTopTab(t, e) {
                const s = document.createElement("li");
                s.classList.add("nav-item", "flex", "border-l", "border-t", "border-r");
                const i = document.createElement("a");
                return i.classList.add("nav-link", "-mb-px", "flex-row", "text-center", "bg-white", "p-2", "hover:bg-blue-400", "rounded-t"), i.setAttribute("href", "#" + e), i.setAttribute("data-toggle", "tab"), i.appendChild(t), s.appendChild(i), s
            }

            getTabContent() {
                const t = document.createElement("div");
                return t.setAttribute("role", "tabpanel"), t
            }

            getTopTabContent() {
                const t = document.createElement("div");
                return t.setAttribute("role", "tabpanel"), t
            }

            markTabActive(t) {
                t.tab.firstChild.classList.add("block"), !0 === t.tab.firstChild.classList.contains("border-b") ? (t.tab.firstChild.classList.add("border-b-0"), t.tab.firstChild.classList.remove("border-b")) : t.tab.firstChild.classList.add("border-b-0"), !0 === t.container.classList.contains("hidden") ? (t.container.classList.remove("hidden"), t.container.classList.add("block")) : t.container.classList.add("block")
            }

            markTabInactive(t) {
                !0 === t.tab.firstChild.classList.contains("border-b-0") ? (t.tab.firstChild.classList.add("border-b"), t.tab.firstChild.classList.remove("border-b-0")) : t.tab.firstChild.classList.add("border-b"), !0 === t.container.classList.contains("block") && (t.container.classList.remove("block"), t.container.classList.add("hidden"))
            }

            getProgressBar() {
                const t = document.createElement("div");
                t.classList.add("progress");
                const e = document.createElement("div");
                return e.classList.add("bg-blue", "leading-none", "py-1", "text-xs", "text-center", "text-white"), e.setAttribute("role", "progressbar"), e.setAttribute("aria-valuenow", 0), e.setAttribute("aria-valuemin", 0), e.setAttribute("aria-valuenax", 100), e.innerHTML = "0%", t.appendChild(e), t
            }

            updateProgressBar(t, e) {
                if (!t) return;
                const s = t.firstChild, i = e + "%";
                s.setAttribute("aria-valuenow", e), s.style.width = i, s.innerHTML = i
            }

            updateProgressBarUnknown(t) {
                if (!t) return;
                const e = t.firstChild;
                t.classList.add("progress", "bg-blue", "leading-none", "py-1", "text-xs", "text-center", "text-white", "block"), e.removeAttribute("aria-valuenow"), e.classList.add("w-full"), e.innerHTML = ""
            }

            getInputGroup(t, e) {
                if (!t) return;
                const s = document.createElement("div");
                s.classList.add("relative", "items-stretch", "w-full"), s.appendChild(t);
                const i = document.createElement("div");
                i.classList.add("-mr-1"), s.appendChild(i);
                for (let t = 0; t < e.length; t++) i.appendChild(e[t]);
                return s
            }
        }

        Y.rules = {
            ".slider": "-webkit-appearance:none;-moz-appearance:none;appearance:none;background:transparent;display:block;border:none;height:1.2rem;width:100%25",
            ".slider:focus": "box-shadow:0%200%200%200%20rgba(87%2C%2085%2C%20217%2C%200.2);outline:none",
            ".slider.tooltip:not([data-tooltip])::after": "content:attr(value)",
            ".slider::-webkit-slider-thumb": "-webkit-appearance:none;background:%23f17405;border-radius:100%25;height:0.6rem;margin-top:-0.25rem;transition:transform%200.2s;width:0.6rem",
            ".slider:active::-webkit-slider-thumb": "transform:scale(1.25);outline:none",
            ".slider::-webkit-slider-runnable-track": "background:%23b2b4b6;border-radius:0.1rem;height:0.1rem;width:100%25",
            "a.tooltips": "position:relative;display:inline",
            "a.tooltips span": "position:absolute;white-space:nowrap;width:auto;padding-left:1rem;padding-right:1rem;color:%23ffffff;background:rgba(56%2C%2056%2C%2056%2C%200.85);height:1.5rem;line-height:1.5rem;text-align:center;visibility:hidden;border-radius:3px",
            "a.tooltips span:after": "content:%22%22;position:absolute;top:50%25;left:100%25;margin-top:-5px;width:0;height:0;border-left:5px%20solid%20rgba(56%2C%2056%2C%2056%2C%200.85);border-top:5px%20solid%20transparent;border-bottom:5px%20solid%20transparent",
            "a:hover.tooltips span": "visibility:visible;opacity:0.9;font-size:0.8rem;right:100%25;top:50%25;margin-top:-12px;margin-right:10px;z-index:999",
            ".json-editor-btntype-properties + div": "font-size:0.8rem;font-weight:normal",
            textarea: "width:100%25;min-height:2rem;resize:vertical",
            table: "width:100%25;border-collapse:collapse",
            ".table td": "padding:0rem%200rem",
            "div[data-schematype]:not([data-schematype='object'])": "transition:0.5s",
            "div[data-schematype]:not([data-schematype='object']):hover": "background-color:%23e6f4fe",
            "div[data-schemaid='root']": "position:relative;width:inherit;display:inherit;overflow-x:hidden;z-index:10",
            "select[multiple]": "height:auto",
            "select[multiple].from-select": "height:auto",
            ".je-table-zebra:nth-child(even)": "background-color:%23f2f2f2",
            ".je-table-border": "border:0.5px%20solid%20black",
            ".je-table-hdiv": "border-bottom:1px%20solid%20black",
            ".je-border": "border:0.05rem%20solid%20%233182ce",
            ".je-panel": "width:inherit;padding:0.2rem;margin:0.2rem;background-color:rgba(218%2C%20222%2C%20228%2C%200.1)",
            ".je-panel-top": "width:100%25;padding:0.2rem;margin:0.2rem;background-color:rgba(218%2C%20222%2C%20228%2C%200.1)",
            ".required:after": "content:%22%20*%22;color:red;font:inherit;font-weight:bold",
            ".je-desc": "font-size:smaller;margin:0.2rem%200",
            ".container-xl.je-noindent": "padding-left:0;padding-right:0",
            ".json-editor-btntype-add": "color:white;margin:0.3rem;padding:0.3rem%200.8rem;background-color:%234299e1;box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2);-webkit-box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2);-moz-box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2)",
            ".json-editor-btntype-deletelast": "color:white;margin:0.3rem;padding:0.3rem%200.8rem;background-color:%23e53e3e;box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2);-webkit-box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2);-moz-box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2)",
            ".json-editor-btntype-deleteall": "color:white;margin:0.3rem;padding:0.3rem%200.8rem;background-color:%23000000;box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2);-webkit-box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2);-moz-box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2)",
            ".json-editor-btn-save": "float:right;color:white;margin:0.3rem;padding:0.3rem%200.8rem;background-color:%232b6cb0;box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2);-webkit-box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2);-moz-box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2)",
            ".json-editor-btn-back": "color:white;margin:0.3rem;padding:0.3rem%200.8rem;background-color:%232b6cb0;box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2);-webkit-box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2);-moz-box-shadow:3px%203px%205px%201px%20rgba(4%2C%204%2C%204%2C%200.2)",
            ".json-editor-btntype-delete": "color:%23e53e3e;background-color:rgba(218%2C%20222%2C%20228%2C%200.1);margin:0.03rem;padding:0.1rem",
            ".json-editor-btntype-move": "color:%23000000;background-color:rgba(218%2C%20222%2C%20228%2C%200.1);margin:0.03rem;padding:0.1rem",
            ".json-editor-btn-collapse": "padding:0em%200.8rem;font-size:1.3rem;color:%23e53e3e;background-color:rgba(218%2C%20222%2C%20228%2C%200.1)",
            ".je-upload-preview img": "float:left;margin:0%200.5rem%200.5rem%200;max-width:100%25;max-height:5rem",
            ".je-dropzone": "position:relative;margin:0.5rem%200;border:2px%20dashed%20black;width:100%25;height:60px;background:teal;transition:all%200.5s",
            ".je-dropzone:before": "position:absolute;content:attr(data-text);color:rgba(0%2C%200%2C%200%2C%200.6);left:50%25;top:50%25;transform:translate(-50%25%2C%20-50%25)",
            ".je-dropzone.valid-dropzone": "background:green",
            ".je-dropzone.invalid-dropzone": "background:red"
        };
        const X = {html: z, bootstrap3: q, bootstrap4: G, jqueryui: U, barebones: J, spectre: Z, tailwind: Y};

        class K {
            constructor(t, e = {}) {
                if (!(t instanceof Element)) throw new Error("element should be an instance of Element");
                this.element = t, this.options = c({}, K.defaults.options, e), this.ready = !1, this.copyClipboard = null, this.schema = this.options.schema, this.template = this.options.template, this.translate = this.options.translate || K.defaults.translate, this.uuid = 0, this.__data = {};
                const s = this.options.theme || K.defaults.theme, i = K.defaults.themes[s];
                if (!i) throw new Error("Unknown theme " + s);
                this.element.setAttribute("data-theme", s), this.theme = new i(this);
                const r = c(i.rules, this.getEditorsRules());
                if (!this.theme.options.disable_theme_rules) {
                    const t = function t(e) {
                        return e && ("[object ShadowRoot]" === e.toString() ? e : t(e.parentNode))
                    }(this.element);
                    this[t ? "addNewStyleRulesToShadowRoot" : "addNewStyleRules"](s, r, t)
                }
                const o = K.defaults.iconlibs[this.options.iconlib || K.defaults.iconlib];
                o && (this.iconlib = new o), this.root_container = this.theme.getContainer(), this.element.appendChild(this.root_container);
                const n = document.location.origin + document.location.pathname.toString(), a = new f(this.options),
                    l = document.location.toString();
                this.expandSchema = (t, e) => a.expandSchema(t, e), this.expandRefs = (t, e) => a.expandRefs(t, e), this.refs = a.refs, a.load(this.schema, t => {
                    const e = this.options.custom_validators ? {custom_validators: this.options.custom_validators} : {};
                    this.validator = new g(this, null, e, K.defaults);
                    const s = this.getEditorClass(t);
                    this.root = this.createEditor(s, {
                        jsoneditor: this,
                        schema: t,
                        required: !0,
                        container: this.root_container
                    }), this.root.preBuild(), this.root.build(), this.root.postBuild(), u(this.options, "startval") && this.root.setValue(this.options.startval), this.validation_results = this.validator.validate(this.root.getValue()), this.root.showValidationErrors(this.validation_results), this.ready = !0, window.requestAnimationFrame(() => {
                        this.ready && (this.validation_results = this.validator.validate(this.root.getValue()), this.root.showValidationErrors(this.validation_results), this.trigger("ready"), this.trigger("change"))
                    })
                }, n, l)
            }

            getValue() {
                if (!this.ready) throw new Error("JSON Editor not ready yet.  Listen for 'ready' event before getting the value");
                return this.root.getValue()
            }

            setValue(t) {
                if (!this.ready) throw new Error("JSON Editor not ready yet.  Listen for 'ready' event before setting the value");
                return this.root.setValue(t), this
            }

            validate(t) {
                if (!this.ready) throw new Error("JSON Editor not ready yet.  Listen for 'ready' event before validating");
                return 1 === arguments.length ? this.validator.validate(t) : this.validation_results
            }

            destroy() {
                this.destroyed || this.ready && (this.schema = null, this.options = null, this.root.destroy(), this.root = null, this.root_container = null, this.validator = null, this.validation_results = null, this.theme = null, this.iconlib = null, this.template = null, this.__data = null, this.ready = !1, this.element.innerHTML = "", this.element.removeAttribute("data-theme"), this.destroyed = !0)
            }

            on(t, e) {
                return this.callbacks = this.callbacks || {}, this.callbacks[t] = this.callbacks[t] || [], this.callbacks[t].push(e), this
            }

            off(t, e) {
                if (t && e) {
                    this.callbacks = this.callbacks || {}, this.callbacks[t] = this.callbacks[t] || [];
                    const s = [];
                    for (let i = 0; i < this.callbacks[t].length; i++) this.callbacks[t][i] !== e && s.push(this.callbacks[t][i]);
                    this.callbacks[t] = s
                } else t ? (this.callbacks = this.callbacks || {}, this.callbacks[t] = []) : this.callbacks = {};
                return this
            }

            trigger(t, e) {
                if (this.callbacks && this.callbacks[t] && this.callbacks[t].length) for (let s = 0; s < this.callbacks[t].length; s++) this.callbacks[t][s].apply(this, [e]);
                return this
            }

            setOption(t, e) {
                if ("show_errors" !== t) throw new Error(`Option ${t} must be set during instantiation and cannot be changed later`);
                return this.options.show_errors = e, this.onChange(), this
            }

            getEditorsRules() {
                return Object.values(K.defaults.editors).reduce((t, e) => e.rules ? c(t, e.rules) : t, {})
            }

            getEditorClass(t) {
                let e;
                if (t = this.expandSchema(t), K.defaults.resolvers.find(s => (e = s(t), e && K.defaults.editors[e])), !e) throw new Error("Unknown editor for schema " + JSON.stringify(t));
                if (!K.defaults.editors[e]) throw new Error("Unknown editor " + e);
                return K.defaults.editors[e]
            }

            createEditor(t, e, s = 1) {
                return new t(e = c({}, t.options || {}, e), K.defaults, s)
            }

            onChange() {
                if (this.ready && !this.firing_change) return this.firing_change = !0, window.requestAnimationFrame(() => {
                    this.firing_change = !1, this.ready && (this.validation_results = this.validator.validate(this.root.getValue()), "never" !== this.options.show_errors ? this.root.showValidationErrors(this.validation_results) : this.root.showValidationErrors([]), this.trigger("change"))
                }), this
            }

            compileTemplate(t, e = K.defaults.template) {
                let s;
                if ("string" == typeof e) {
                    if (!K.defaults.templates[e]) throw new Error("Unknown template engine " + e);
                    if (s = K.defaults.templates[e](), !s) throw new Error(`Template engine ${e} missing required library.`)
                } else s = e;
                if (!s) throw new Error("No template engine set");
                if (!s.compile) throw new Error("Invalid template engine set");
                return s.compile(t)
            }

            _data(t, e, s) {
                if (3 !== arguments.length) return t.hasAttribute("data-jsoneditor-" + e) ? this.__data[t.getAttribute("data-jsoneditor-" + e)] : null;
                {
                    let i;
                    t.hasAttribute("data-jsoneditor-" + e) ? i = t.getAttribute("data-jsoneditor-" + e) : (i = this.uuid++, t.setAttribute("data-jsoneditor-" + e, i)), this.__data[i] = s
                }
            }

            registerEditor(t) {
                return this.editors = this.editors || {}, this.editors[t.path] = t, this
            }

            unregisterEditor(t) {
                return this.editors = this.editors || {}, this.editors[t.path] = null, this
            }

            getEditor(t) {
                if (this.editors) return this.editors[t]
            }

            watch(t, e) {
                return this.watchlist = this.watchlist || {}, this.watchlist[t] = this.watchlist[t] || [], this.watchlist[t].push(e), this
            }

            unwatch(t, e) {
                if (!this.watchlist || !this.watchlist[t]) return this;
                if (!e) return this.watchlist[t] = null, this;
                const s = [];
                for (let i = 0; i < this.watchlist[t].length; i++) this.watchlist[t][i] !== e && s.push(this.watchlist[t][i]);
                return this.watchlist[t] = s.length ? s : null, this
            }

            notifyWatchers(t) {
                if (!this.watchlist || !this.watchlist[t]) return this;
                for (let e = 0; e < this.watchlist[t].length; e++) this.watchlist[t][e]()
            }

            isEnabled() {
                return !this.root || this.root.isEnabled()
            }

            enable() {
                this.root.enable()
            }

            disable() {
                this.root.disable()
            }

            setCopyClipboardContents(t) {
                this.copyClipboard = t
            }

            getCopyClipboardContents() {
                return this.copyClipboard
            }

            addNewStyleRules(t, e) {
                let s = document.querySelector("#theme-" + t);
                s || (s = document.createElement("style"), s.setAttribute("id", "theme-" + t), s.appendChild(document.createTextNode("")), document.head.appendChild(s));
                const i = s.sheet ? s.sheet : s.styleSheet, r = this.element.nodeName.toLowerCase();
                Object.keys(e).forEach(s => {
                    const o = `${r}[data-theme="${t}"] ${s}`;
                    i.insertRule ? i.insertRule(o + " {" + decodeURIComponent(e[s]) + "}", 0) : i.addRule && i.addRule(o, decodeURIComponent(e[s]), 0)
                })
            }

            addNewStyleRulesToShadowRoot(t, e, s) {
                const i = this.element.nodeName.toLowerCase();
                let r = "";
                Object.keys(e).forEach(s => {
                    r += `${i}[data-theme="${t}"] ${s}` + " {" + decodeURIComponent(e[s]) + "}\n"
                });
                const o = new CSSStyleSheet;
                o.replaceSync(r), s.adoptedStyleSheets = [...s.adoptedStyleSheets, o]
            }
        }

        K.defaults = a, K.AbstractEditor = _, K.AbstractTheme = P, K.AbstractIconLib = I, Object.assign(K.defaults.themes, X), Object.assign(K.defaults.editors, L), Object.assign(K.defaults.templates, T), Object.assign(K.defaults.iconlibs, H)
    }])
}));