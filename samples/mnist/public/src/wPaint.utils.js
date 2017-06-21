(function () {
  if (!String.prototype.capitalize) {
    String.prototype.capitalize = function () {
      return this.slice(0, 1).toUpperCase() + this.slice(1);
    };
  }
})();

(function ($) {
  $.fn.bindMobileEvents = function () {
    $(this).on('touchstart touchmove touchend touchcancel', function () {
      var touches = (event.changedTouches || event.originalEvent.targetTouches),
          first = touches[0],
          type = '';

      switch (event.type) {
      case 'touchstart':
        type = 'mousedown';
        break;
      case 'touchmove':
        type = 'mousemove';
        event.preventDefault();
        break;
      case 'touchend':
        type = 'mouseup';
        break;
      default:
        return;
      }

      var simulatedEvent = document.createEvent('MouseEvent'); 

      simulatedEvent.initMouseEvent(
        type, true, true, window, 1, 
        first.screenX, first.screenY, first.clientX, first.clientY, 
        false, false, false, false, 0/*left*/, null
      );

      first.target.dispatchEvent(simulatedEvent);
    });
  };
})(jQuery);