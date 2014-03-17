;(function () {
  Velox.Facebook = {};

  Velox.Facebook.isReady = false;
  Velox.Facebook.readyCallbacks = [];

  Velox.Facebook.ready = function (callback) {
    if (this.isReady) {
      callback.apply(this);
    }
    else {
      this.readyCallbacks.push(callback);
    }
  };

  Velox.Facebook.inCanvas = function () {
    return (window.location.search.indexOf('fb_sig_in_iframe=1') > -1) ||
           (window.location.search.indexOf('session=') > -1) ||
           (window.location.search.indexOf('signed_request=') > -1) ||
           (window.name.indexOf('iframe_canvas') > -1) ||
           (window.name.indexOf('app_runner') > -1);
  };

  Velox.Facebook.getAppId = function () {
    return Velox.Settings.VeloxFacebook.appId;
  };

  Velox.Facebook.getLocale = function () {
    return Velox.Settings.VeloxFacebook.locale;
  };

  Velox.Facebook.getChannelUrl = function () {
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

    Velox.Facebook.isReady = true;
    var readyCallbacks = Velox.Facebook.readyCallbacks;
    
    if (readyCallbacks.length) {
      for (var i=0, c=readyCallbacks.length; i<c; i++) {
        readyCallbacks[i].apply(Velox.Facebook);
      }
    }
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
