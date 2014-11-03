;(function () {

  Velox.Facebook = {};

  Velox.Facebook.isReady = false;
  Velox.Facebook.pageInfoChangeInterval = 50;
  Velox.Facebook.pageInfoChangeEnabled = false;
  Velox.Facebook.lastPageInfo = {};
  Velox.Facebook.events = {
    ready: [],
    pageInfoChange: []
  };

  Velox.Facebook.pageInfoChange = function() {
    var that = this;

    FB.Canvas.getPageInfo(function(info) {
      // Loop trough all info and see if any properties has changed.
      for (var k in info) {
        if (that.lastPageInfo[k] !== info[k]) {
          that.lastPageInfo = info;
          that.trigger('pageInfoChange', info);
          break;
        }
      }

      setTimeout(function() {
        that.pageInfoChange();
      }, that.pageInfoChangeInterval);
    });
  };

  Velox.Facebook.on = function(event, callback) {
    if (event == 'ready') {
      if (this.isReady) {
        callback.apply(this);
        return;
      }
    }
    else if (event == 'pageInfoChange' && this.isReady) {
      this.pageInfoChangeEnabled = true;
      this.pageInfoChange();
    }

    this.events[event].push(callback);
  };

  Velox.Facebook.trigger = function(event, data) {
    if (event == 'ready') {
      if (
        this.events.pageInfoChange.length &&
        !this.pageInfoChangeEnabled
      ) {
        this.pageInfoChangeEnabled = true;
        this.pageInfoChange();
      }

      this.isReady = true;
    }

    for (var i=0, c=this.events[event].length; i<c; i++) {
      this.events[event][i].call(this, data);
    }
  };

  Velox.Facebook.inCanvas = function() {
    return (window.location.search.indexOf('fb_sig_in_iframe=1') > -1) ||
           (window.location.search.indexOf('session=') > -1) ||
           (window.location.search.indexOf('signed_request=') > -1) ||
           (window.name.indexOf('iframe_canvas') > -1) ||
           (window.name.indexOf('app_runner') > -1);
  };

  Velox.Facebook.getAppId = function() {
    return Velox.Settings.VeloxFacebook.appId;
  };

  Velox.Facebook.getLocale = function() {
    return Velox.Settings.VeloxFacebook.locale;
  };

  Velox.Facebook.getChannelUrl = function() {
    return Velox.Settings.VeloxFacebook.channelUrl;
  };

  window.fbAsyncInit = function() {
    // init the FB JS SDK
    FB.init({
      appId      : Velox.Facebook.getAppId(),
      channelUrl : Velox.Facebook.getChannelUrl(),
      status     : true,
      xfbml      : true,
      cookie     : true
    });

    Velox.Facebook.trigger('ready');
  };

  // Load the SDK asynchronously
  (function(d, s, id){
    var js, fjs = d.getElementsByTagName(s)[0];
    if (d.getElementById(id)) {return;}
    js = d.createElement(s); js.id = id;
    js.src = '//connect.facebook.net/' + Velox.Facebook.getLocale() + '/all.js';
    fjs.parentNode.insertBefore(js, fjs);
  }(document, 'script', 'facebook-jssdk'));

})();
