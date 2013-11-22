<?php
namespace Memcache_UI\Core {
  class Context {
    protected $logLevelClasses = NULL;

    /**
     * Base URL for the script, for HTTP paths
     *
     * @var string
     */
    protected $base;

    /**
     * Base directory for the script, for file operations
     */
    protected $directory;

    /**
     * Graphics context
     *
     * @var GraphicsContext
     */
    protected $gc = NULL;

    /**
     * Locale with encoding
     *
     * This is a static because a page runs for only one locale.
     */
    protected static $locale = 'en_US.UTF8';

    /**
     * Logging level, as per RFC5424
     *
     * @link http://php.net/network.constants.php
     *
     * @var integer
     */
    protected $logLevel = NULL;

    /**
     * Messages for display.
     *
     * @var array
     */
    protected $messages = array();

    /**
     * Requested path: <$PHP_SELF>?q=a/b/c
     *
     * @var string
     */
    protected $path = NULL;

    /**
     * Tidy the output ?
     *
     * @var boolean
     */
    protected $tidy = NULL;

    /**
     * User information: logged or not ?
     *
     * @var boolean
     */
    protected $user = FALSE;

    function __construct() {
      $this->initLocale(); // Check extension and initialize locale
      $this->getTidy(); // Needed to check optional extension
    }

    function __destruct() {
      if (!empty($this->messages)) {
        $ret = (string) new Element('pre', array('class' => array('messages')),
          implode("\n", $this->getMessage(TRUE)));
        echo $ret;
      }
    }

    function __toString() {
      $ret = '<pre>' . print_r($this, TRUE) . '</pre>';
      return $ret;
    }

    /**
     * Get the base path where the script is located.
     *
     * This is helpful to locate other files using paths relative to the install.
     */
    public function getBase() {
      if (!isset($this->base)) {
        $this->base = dirname($_SERVER['SCRIPT_NAME']);
        if ($this->base == '/') {
          $this->base = '';
        }
      }

      return $this->base;
    }

    public function getDirectory() {
      if (!isset($this->directory)) {
        $this->directory = dirname($_SERVER['SCRIPT_FILENAME']);
      }

      return $this->directory;
    }

    public function getLogLevel() {
      if (!isset($this->logLevel)) {
        $usLogLevel = NULL;
        foreach ($_GET as $key => $value) {
          if (strtolower($key) === 'loglevel') {
            $usLogLevel = (int) $value;
            break;
          }
        }
        if (!isset($usLogLevel)) {
          $usLogLevel = LOG_NOTICE;
        }

        if ($usLogLevel < LOG_EMERG) {
          $this->logLevel = LOG_EMERG;
        }
        elseif ($usLogLevel > LOG_DEBUG) {
          $this->logLevel = LOG_DEBUG;
        }
        else {
          $this->logLevel = $usLogLevel; // We now know it to be safe
        }
      }

      return $this->logLevel;
    }

    public function getLogLevelClass($logLevel) {
      if (!isset($this->logLevelClasses)) {
        $this->logLevelClasses = array(
          LOG_EMERG   => 'error',
          LOG_ALERT   => 'error',
          LOG_CRIT    => 'error',
          LOG_ERR     => 'error',
          LOG_WARNING => 'warning',
          LOG_NOTICE  => 'warning',
          LOG_INFO    => 'status',
          LOG_DEBUG   => 'status',
        );
      }
      if ($logLevel < LOG_EMERG) {
        $logLevel = LOG_EMERG;
      }
      elseif ($logLevel > LOG_DEBUG) {
        $logLevel = LOG_DEBUG;
      }
      return $this->logLevelClasses[$logLevel];
    }

    public function getMessage($clear = FALSE) {
      $ret = $this->messages;
      if ($clear) {
        $this->messages = array();
      }

      return $ret;
    }

    /**
     * Return the requested path.
     *
     * @param string $path
     */
    public function getPath() {
      if (!isset($this->path)) {
        $this->path = empty($_GET['q']) ? '' : $_GET['q'];
      }

      return $this->path;
    }

    /**
     * Return the "tidy" status.
     *
     * Will only be TRUE if requested (possibly by default) and extension is
     * loaded. A warning will be generated if tidy is requested but extension is
     * not loaded.
     */
    public function getTidy() {
      static $notified = FALSE;
      if (!isset($this->tidy)) {
        $this->tidy = TRUE;
        foreach ($_GET as $key => $value) {
          if (strtolower($key) === 'tidy') {
            $this->tidy = !!$value;
            break;
          }
        }
        if (!$notified && $this->tidy && !extension_loaded('tidy')) {
          $this->setMessage(t('Extension @tidy requested but missing: output formatting unavailable.', array(
            '@tidy' => 'tidy',
          )), LOG_WARNING);
          $notified = TRUE;
          $this->tidy = FALSE;
        }
      }

      return $this->tidy;
    }

    /**
     * Initialize the locale based on the user Accept-Language header.
     *
     * @TODO support "xx" form, and not just "xx_YY".
     * @TODO support more platforms. UNIX and MacOS X do not handle locales like Linux
     *
     * @link @link http://www.php.net/manual/fr/function.bind-textdomain-codeset.php#42631
     *
     * @return void
     */
    protected function initLocale() {
      static $notified = FALSE;
      if (!$notified && !extension_loaded('intl')) {
        // Do not invoke t() before initializing locale
        $this->setMessage(strtr('Extension @intl requested but missing: translations unavailable.', array(
          '@intl' => 'intl',
        )), LOG_WARNING);
      }
      else {
        $locale = \Locale::acceptFromHttp($_SERVER['HTTP_ACCEPT_LANGUAGE']);

        $this->setMessage(strtr('Requested locale: @locale', array('@locale' => $locale)), LOG_DEBUG);
        if (!empty($locale)) {
          $matches = array();
          $count = preg_match('/^([a-z]{2}_[A-Z]{2})(\.(\w+))*/', $locale, $matches);
          if ($count) {
            $count = count($matches);
          }
          $locale = $count >= 2
            ? $matches[1]
            : 'en_US';
          $codeset = $count >= 4
            ? $matches[3]
            : 'UTF8';
          self::$locale = $locale . '.' . $codeset;
          setlocale(LC_ALL, self::$locale);

          $domain = 'messages';
          $translation_path = 'locale';
          bindtextdomain($domain, $translation_path);
          textdomain($domain);
          bind_textdomain_codeset($domain, 'UTF8');
          $this->setMessage(self::t('Locale: @locale', array('@locale' => self::$locale)), LOG_DEBUG);
        }
      }
    }

    /**
     * Add a message to the messages list if it is above the current logging level.
     *
     * @param string $text
     * @param integer $logLevel
     *
     * @return void
     */
    public function setMessage($text, $logLevel = LOG_NOTICE) {
      if ($logLevel <= $this->getlogLevel()) {
        if (is_array($text) || (is_object($text) && !method_exists($text, '__toString'))) {
          $this->messages[] = array('<pre>' . print_r($text, TRUE) . '</pre>', $logLevel);
        }
        else {
          $this->messages[] = array((string) $text, $logLevel);
        }
      }
    }

    /**
     * Wrapper to combine gettext translation and parameter substitution.
     *
     * @param string $message
     * @param array $args
     *
     * @return string
     */
    static function t($message, $args = array()) {
      return strtr(gettext($message), $args);
    }

    /**
     * Embryonic version of Drupal url().
     *
     * @param string $path
     * @param array $options
     *   - No option currently recognized.
     *   - 'absolute' likely at some point
     *
     * @return string
     */
    static function url($path = NULL, $options = array()) {
      $options += array(
        // 'absolute' => FALSE,
      );

    return $this->getBase() . '?q=' . urlencode($path);
    }
  }
}
