/**
 * @file
 * Adds functionality required by the Facemelt module.
 */

var Facemelt = Facemelt || {};
Facemelt.openFacebookConnectDialog = function (opts, state, cb) {
  if (typeof opts != 'object') opts = {};

  FB.login(function (response) {
    if (cb) { cb(response); }
  },
  { display: opts.display || Facemelt.loginDisplay,
    redirect_uri: opts.redirect_uri || (Facemelt.loginRedirectUri + '?destination=fboauth-done'),
    response_type: opts.response_type || 'code',
    scope: (opts.scope !== null ? opts.scope : Facemelt.loginOAuthScope),
    state: state
  });
};

// laziness method
Facemelt.openFacebookLoginDialog = function (state, cb) {
  FB.login(function (response) {
    if (cb) { cb(response); }
  },
  { display: Facemelt.actions.connect.display,
    redirect_uri: Facemelt.actions.connect.redirect_uri,
    response_type: 'code',
    scope: Facemelt.actions.connect.scope,
    state: state
  });
}

