(function ($) {
  'use strict';

  /************************************************************************
   * Paint class
   ************************************************************************/
  function Paint(el, options) {
    this.$el = $(el);
    this.options = options;
    this.init = false;

    this.menus = {primary: null, active: null, all: {}};
    this.previousMode = null;
    this.width = this.$el.width();
    this.height = this.$el.height();

    this.ctxBgResize = false;
    this.ctxResize = false;

    this.generate();
    this._init();
  }
  
  Paint.prototype = {
    generate: function () {
      if (this.init) { return this; }

      var _this = this;

      // automatically appends each canvas
      // also returns the jQuery object so we can chain events right off the function call.
      // for the tempCanvas we will be setting some extra attributes but don't won't matter
      // as they will be reset on mousedown anyway.
      function createCanvas(name) {
        var newName = (name ? name.capitalize() : ''),
            canvasName = 'canvas' + newName,
            ctxName = 'ctx' + newName;

        _this[canvasName] = document.createElement('canvas');
        _this[ctxName] = _this[canvasName].getContext('2d');
        _this['$' + canvasName] = $(_this[canvasName]);
        
        _this['$' + canvasName]
        .attr('class', 'wPaint-canvas' + (name ? '-' + name : ''))
        .attr('width', _this.width + 'px')
        .attr('height', _this.height + 'px')
        .css({position: 'absolute', left: 0, top: 0});

        _this.$el.append(_this['$' + canvasName]);

        return _this['$' + canvasName];
      }

      // event functions
      function canvasMousedown(e) {
        e.preventDefault();
        e.stopPropagation();
        _this.draw = true;
        e.canvasEvent = 'down';
        _this._callShapeFunc.apply(_this, [e]);
      }

      function documentMousemove(e) {
        if (_this.draw) {
          e.canvasEvent = 'move';
          _this._callShapeFunc.apply(_this, [e]);
        }
      }

      function documentMouseup(e) {

        //make sure we are in draw mode otherwise this will fire on any mouse up.
        if (_this.draw) {
          _this.draw = false;
          e.canvasEvent = 'up';
          _this._callShapeFunc.apply(_this, [e]);
        }
      }

      // create bg canvases
      createCanvas('bg');
      
      // create drawing canvas
      createCanvas('')
      .on('mousedown', canvasMousedown)
      .bindMobileEvents();
      
      // create temp canvas for drawing shapes temporarily
      // before transfering to main canvas
      createCanvas('temp').hide();
      
      // event handlers for drawing
      $(document)
      .on('mousemove', documentMousemove)
      .on('mouseup', documentMouseup);

    },

    _init: function () {
      var index = null,
          setFuncName = null;

      this.init = true;

      // run any set functions if they exist
      for (index in this.options) {
        setFuncName = 'set' + index.capitalize();
        if (this[setFuncName]) { this[setFuncName](this.options[index]); }
      }
    },

    getClear:function () {
      this.ctx.clearRect(0, 0, this.width, this.height);
    },

    getImage: function (withBg) {
      var canvasSave = document.createElement('canvas'),
      ctxSave = canvasSave.getContext('2d');

      withBg = withBg === false ? false : true;

      $(canvasSave)
      .css({display: 'none', position: 'absolute', left: 0, top: 0})
      .attr('width', this.width)
      .attr('height', this.height);

      if (withBg) { ctxSave.drawImage(this.canvasBg, 0, 0); }
      ctxSave.drawImage(this.canvas, 0, 0);

      return canvasSave.toDataURL();
    },

    /************************************
     * events
     ************************************/

    _callShapeFunc: function (e) {

      // TODO: this is where issues with mobile offsets are probably off
      var canvasOffset = this.$canvas.offset(),
          canvasEvent = e.canvasEvent.capitalize(),
          func = '_draw' + this.options.mode.capitalize() + canvasEvent;

      // update offsets here since we are detecting mouseup on $(document) not on the canvas
      e.pageX = Math.floor(e.pageX - canvasOffset.left);
      e.pageY = Math.floor(e.pageY - canvasOffset.top);

      // call drawing func
      if (this[func]) { this[func].apply(this, [e]); }

      // run callback if set
      if (this.options['draw' + canvasEvent]) { this.options['_draw' + canvasEvent].apply(this, [e]); }

      // run options (user) callback if set
      if (canvasEvent === 'Down' && this.options.onShapeDown) { this.options.onShapeDown.apply(this, [e]); }
      else if (canvasEvent === 'Move' && this.options.onShapeMove) { this.options.onShapeMove.apply(this, [e]); }
      else if (canvasEvent === 'Up' && this.options.onShapeUp) { this.options.onShapeUp.apply(this, [e]); }
    }
  };

  /************************************************************************
   * Menu class
   ************************************************************************/
  function Menu(wPaint, name, options) {
    this.wPaint = wPaint;
    this.options = options;
    this.name = name;

    this.generate();
  }
  
  Menu.prototype = {
    generate: function () {
      this.reset();
      
      // append menu
      this.wPaint.$el.append(this.$menu);
    },
  };

  /************************************************************************
   * wPaint
   ************************************************************************/
  $.support.canvas = (document.createElement('canvas')).getContext;

  $.fn.wPaint = function (options, value) {

    function create() {
      if (!$.support.canvas) {
        $(this).html('Browser does not support HTML5 canvas, please upgrade to a more modern browser.');
        return false;
      }

      return $.proxy(get, this)();
    }

    function get() {
      var wPaint = $.data(this, 'wPaint');

      if (!wPaint) {
        wPaint = new Paint(this, $.extend(true, {}, options));
        $.data(this, 'wPaint', wPaint);
      }

      return wPaint;
    }

    function runOpts() {
      var wPaint = $.data(this, 'wPaint');

      if (wPaint) {
        if (wPaint[options]) { wPaint[options].apply(wPaint, [value]); }
        else if (value !== undefined) {
          if (wPaint[func]) { wPaint[func].apply(wPaint, [value]); }
          if (wPaint.options[options]) { wPaint.options[options] = value; }
        }
        else {
          if (wPaint[func]) { values.push(wPaint[func].apply(wPaint, [value])); }
          else if (wPaint.options[options]) { values.push(wPaint.options[options]); }
          else { values.push(undefined); }
        }
      }
    }

    if (typeof options === 'string') {
      var values = [],
          func = (value ? 'set' : 'get') + options.charAt(0).toUpperCase() + options.substring(1);

      this.each(runOpts);

      if (values.length) { return values.length === 1 ? values[0] : values; }
      
      return this;
    }

    options = $.extend({}, $.fn.wPaint.defaults, options);
    options.lineWidth = parseInt(options.lineWidth, 10);
    options.fontSize = parseInt(options.fontSize, 10);

    return this.each(create);
  };

  /************************************************************************
   * extend
   ************************************************************************/
  $.fn.wPaint.extend = function (funcs, protoType) {
    var key;
    
    function elEach(func) {
      if (protoType[func]) {
        var tmpFunc = Paint.prototype[func],
            newFunc = funcs[func];
        
        protoType[func] = function () {
          tmpFunc.apply(this, arguments);
          newFunc.apply(this, arguments);
        };
      }
      else {
        protoType[func] = funcs[func];
      }
    }

    protoType = protoType === 'menu' ? Menu.prototype : Paint.prototype;

    for (key in funcs) { (elEach)(key); }
  };

  /************************************************************************
   * Init holders
   ************************************************************************/
  $.fn.wPaint.menus = {};

  $.fn.wPaint.cursors = {};

  $.fn.wPaint.defaults = {
    path:            '/',                // set absolute path for images and cursors
    theme:           'standard classic', // set theme
    autoScaleImage:  true,               // auto scale images to size of canvas (fg and bg)
    autoCenterImage: true,               // auto center images (fg and bg, default is left/top corner)
    menuHandle:      true,               // setting to false will means menus cannot be dragged around
    menuOrientation: 'horizontal',       // menu alignment (horizontal,vertical)
    menuOffsetLeft:  5,                  // left offset of primary menu
    menuOffsetTop:   5,                  // top offset of primary menu
    bg:              null,               // set bg on init
    image:           null,               // set image on init
    imageStretch:    false,              // stretch smaller images to full canvans dimensions
    onShapeDown:     null,               // callback for draw down event
    onShapeMove:     null,               // callback for draw move event
    onShapeUp:       null                // callback for draw up event
  };
})(jQuery);