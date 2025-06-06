// 本节介绍一下类
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (Object.prototype.hasOwnProperty.call(b, p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        if (typeof b !== "function" && b !== null)
            throw new TypeError("Class extends value " + String(b) + " is not a constructor or null");
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
var Dom = /** @class */ (function () {
    function Dom() {
    }
    Dom.prototype.createElement = function (el) {
        return document.createElement(el);
    };
    Dom.prototype.setText = function (el, text) {
        el.textContent = text;
    };
    Dom.prototype.render = function (data) {
        var _this = this;
        var root = this.createElement(data.tag);
        if (data.children && Array.isArray(data.children)) {
            data.children.forEach(function (item) {
                root.appendChild(_this.render(item));
            });
        }
        else {
            this.setText(root, data.text);
        }
        return root;
    };
    return Dom;
}());
var Vue = /** @class */ (function (_super) {
    __extends(Vue, _super);
    function Vue(options) {
        var _this = _super.call(this) || this;
        _this.options = options;
        _this.init();
        return _this;
    }
    Vue.prototype.init = function () {
        var data = {
            tag: 'div',
            children: [
                {
                    tag: 'h1',
                    text: 'Hello, World!'
                },
                {
                    tag: 'p',
                    text: 'This is a paragraph.'
                }
            ]
        };
        var app = typeof this.options.el === 'string' ? document.querySelector(this.options.el) : this.options.el;
        app.appendChild(this.render(data));
    };
    return Vue;
}(Dom));
new Vue({
    el: '#app'
});
