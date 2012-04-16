/**
 * @file
 * Attaches behaviors for the Facemelt module.
 */

(function ($) {
 
  // add facebook connect popup for links and buttons
  Drupal.behaviors.facemelt_linksAndButtons_promptUserWithFacebookLogin = {
    attach: function (context) {
      if (!Facemelt.userLoggedIn) {
        $('.facemelt-prompt-fb-connect:not(.facemelt-prompt-fb-connect-processed)')
          .filter('a, :button')
          .addClass('facemelt-prompt-fb-connect-processed')
          .each(function() {
            var $link = $(this);
            $link.click(function (evt) {
              evt.preventDefault();
              evt.stopImmediatePropagation();

              var destination = $link.attr('href') || $link.attr('data-redirect-href');

              Facemelt.openFacebookLoginDialog(null, function (response) {
                // go to the link's original destination
                window.location.replace(destination);
              });
              return false;
            }); // click()
          }); // each()
      } // if (!Facemelt.userLoggedIn)
    } // attach
  };
  
  // add facebook connect popup for forms
  Drupal.behaviors.facemelt_forms_promptUserWithFacebookLogin = {
    attach: function (context) {
      if (!Facemelt.userLoggedIn) {
        $('form.facemelt-prompt-fb-connect:not(.facemelt-prompt-fb-connect-processed)')
          .addClass('facemelt-prompt-fb-connect-processed')
          .each(function() {
            var $form = $(this);
            $form.submit(function (evt) {
              evt.preventDefault();
              evt.stopImmediatePropagation();

              // remove the build id from the form -- this can be a security concern
              $('input[name=form_build_id]', $form).remove();

              // move the contents of any tinyMCE elements into their real form elements
              if (!!Facemelt.FBConnect && typeof Facemelt.FBConnect.tinymce_elements != 'undefined') {
                for (var i in Facemelt.FBConnect.tinymce_elements) {
                  var id = Facemelt.FBConnect.tinymce_elements[i];
                  var tinymceText = tinymce.get(id).getContent();
                  $('#' + tinymce.get(id).id).val(tinymceText);
                }
              }

              var state = $form.serialize();
              var requestIdentifier = $form.attr('data-request-identifier');
              if (requestIdentifier) state += '&' + requestIdentifier;
              
              Facemelt.openFacebookLoginDialog(state, function (response) {
                // @@TODO: make this path a variable?
                $.post('/store-in-session', state, function (data, textStatus, jqXHR) {
                  // reload the page
                  window.location.replace(window.location);
                });
              });
              return false;
            }); // click()
          }); // each()
      } // if (!Facemelt.userLoggedIn)
    } // attach
  };

})(jQuery);


