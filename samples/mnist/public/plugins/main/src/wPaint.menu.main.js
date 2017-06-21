(function ($) {

  // extend defaults
  $.extend($.fn.wPaint.defaults, {
    mode:        'pencil',  // set mode
    lineWidth:   '30',       // starting line width
    fillStyle:   'black', // starting fill style
    strokeStyle: 'black'  // start stroke style
  });

  // extend functions
  $.fn.wPaint.extend({
   
    /****************************************
     * pencil
     ****************************************/
    _drawPencilDown: function (e) {
      this.ctx.lineJoin = 'round';
      this.ctx.lineCap = 'round';
      this.ctx.strokeStyle = this.options.strokeStyle;
      this.ctx.fillStyle = this.options.strokeStyle;
      this.ctx.lineWidth = this.options.lineWidth;
      
      //draw single dot in case of a click without a move
      this.ctx.beginPath();
      this.ctx.arc(e.pageX, e.pageY, this.options.lineWidth / 2, 0, Math.PI * 2, true);
      this.ctx.closePath();
      this.ctx.fill();
      
      //start the path for a drag
      this.ctx.beginPath();
      this.ctx.moveTo(e.pageX, e.pageY);
    },
    
    _drawPencilMove: function (e) {
      this.ctx.lineTo(e.pageX, e.pageY);
      this.ctx.stroke();
    },
    
    _drawPencilUp: function () {
      this.ctx.closePath();
    },

  });
})(jQuery);
