<?php


class facemelt_facebook extends Facebook {
  
  public function restserver($params, $json_decode = TRUE, $use_access_token = TRUE) {
    return $this->_restserver($params, $json_decode, $use_access_token);
  }
  
  protected function getApiUrl($method) {
    if ($method == 'fql.query') {
      return self::getUrl('api_read', 'method/fql.query');
    }
    
    return parent::getApiUrl($method);
  }

  /**
   * Invoke the old restserver.php endpoint.
   *
   * @param Array $params method call object
   * @return the decoded response object
   * @throws FacebookApiException
   */
  protected function _restserver($params, $json_decode = TRUE, $use_access_token = TRUE) {
    // generic application level parameters
    $params['api_key'] = $this->getAppId();
    $params['format'] = 'json-strings';

    if ($use_access_token) {
      $result = $this->_oauthRequest(
        $this->getApiUrl($params['method']),
        $params
      );
    }
    else {
      $_params = unserialize(serialize($params));
      foreach ($_params as $key => $value) {
        if (!is_string($value)) {
          $_params[$key] = json_encode($value);
        }
      }
      $result = $this->makeRequest($this->getApiUrl($params['method']), $_params);
    }

    if ($json_decode) {
      $result = json_decode($result, true);
    }

    // results are returned, errors are thrown
    if (is_array($result) && isset($result['error_code'])) {
      throw new FacebookApiException($result);
    }
    return $result;
  }
  
  
  
  protected function makeRequest($url, $params, $ch=null) {
    if (!$ch) {
      $ch = curl_init();
    }

    $opts = self::$CURL_OPTS;
    if ($this->useFileUploadSupport()) {
      $opts[CURLOPT_POSTFIELDS] = $params;
    } else {
      $opts[CURLOPT_POSTFIELDS] = http_build_query($params, null, '&');
    }
    $opts[CURLOPT_URL] = $url;

    // disable the 'Expect: 100-continue' behaviour. This causes CURL to wait
    // for 2 seconds if the server does not support this header.
    if (isset($opts[CURLOPT_HTTPHEADER])) {
      $existing_headers = $opts[CURLOPT_HTTPHEADER];
      $existing_headers[] = 'Expect:';
      $opts[CURLOPT_HTTPHEADER] = $existing_headers;
    } else {
      $opts[CURLOPT_HTTPHEADER] = array('Expect:');
    }

    curl_setopt_array($ch, $opts);
    $result = curl_exec($ch);

    if (curl_errno($ch) == 60) { // CURLE_SSL_CACERT
      self::errorLog('Invalid or no certificate authority found, using bundled information');
      curl_setopt($ch, CURLOPT_CAINFO,
                  dirname(__FILE__) . '/fb_ca_chain_bundle.crt');
      $result = curl_exec($ch);
    }

    if ($result === false) {
      $e = new FacebookApiException(array(
        'error_code' => curl_errno($ch),
        'error'      => array(
          'message' => curl_error($ch),
          'type'    => 'CurlException',
        ),
      ));
      curl_close($ch);
      throw $e;
    }
    curl_close($ch);
    return $result;
  }
}
