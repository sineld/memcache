<?php
/**
 * @file
 * Load configuration from defaults or a configuration file
 */

namespace Memcache_UI\Core {

  class Config {
    /**
     * The single account allowed access to the UI.
     *
     * @var string
     */
    public $account = NULL;

    /**
     * The cleartext password for the authorized account.
     *
     * @var string
     */
    public $password = NULL;

    /**
     * The list of servers to monitor.
     *
     * @var array
     *   Entries in "server:port" format
     */
    public $servers = array(
      'localhost:11211',
    );

    public function __construct(Context $context) {
      $path = $context->getDirectory() . '/memcache_ui.local.php';
      if (file_exists($path)) {
        if (is_readable($path)) {
          $context->setMessage(Context::t('Loading config from @path', array('@path' => $path)), LOG_DEBUG);
          require_once($path);
          $context->setMessage($this, LOG_DEBUG);

          if (empty($this->account)) {
            $context->setMessage(Context::t('Empty account: anonymous access only.'), LOG_ERR);
          }
          elseif (empty($this->password)) {
            $context->setMessage(Context::t('Empty password: anonymous access only.'), LOG_ERR);
          }
          elseif (empty($this->servers)) {
            $context->setMessage(Context::t('Empty servers array: no server info.'), LOG_ERR);
          }
        }
        else {
          $context->setMessage(Context::t('Local config @path exists but is not readable', array(
            '@path' => $path,
          )), LOG_ERR);
        }
      }
      else {
        $context->setMessage(Context::t('Local config @path does not exist.', array(
          '@path' => $path,
        )), LOG_ERR);
      }
    }
  }
}
