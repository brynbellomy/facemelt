# FACEMELT

## Facebook Connect + Open Graph helpers for Drupal 7

This makes all of that nonsense a __LOT__ easier.  Just `drush dl facemelt` or,
preferably, clone the git repo (it's more frequently updated).

### Facebook Connect

Facemelt offers several different modes of integrating with Facebook Connect,
namely, via __regular links__, __form submissions__, and __Drupal paths__.

#### Links

Just add the `facemelt-prompt-fb-connect` class.

```html
<a href="#" class="facemelt-prompt-fb-connect">Login with Facebook</a>
```

#### Form submissions

Same thing.  Just add the `facemelt-prompt-fb-connect` class.  The cool thing
here is that Facemelt offers a hook API that allows Drupal to stash the form
values while the user goes through the FB Connect flow and process them
immediately AFTER the user's Drupal account has been created.

## Errata

Drupal.org project page: http://drupal.org/projecs/facemelt

<img src="http://www.threadbombing.com/data/media/2/face_melt.gif" />
<img src="http://www.threadbombing.com/data/media/2/face_melt.gif" />
<img src="http://www.threadbombing.com/data/media/2/face_melt.gif" />

Or, for the real deal, http://www.youtube.com/watch?v=4RCI8D8avGI

# Authors and contributors

bryn austin bellomy <<bryn@signals.io>>

# License (MIT license)

Copyright (c) 2012 bryn austin bellomy, http://signals.io/

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
