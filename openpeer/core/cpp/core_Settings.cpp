/*

 Copyright (c) 2013, SMB Phone Inc.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies,
 either expressed or implied, of the FreeBSD Project.

 */

#include <openpeer/core/internal/core_Settings.h>

#include <openpeer/core/internal/core.h>

#include <openpeer/services/IHelper.h>

#include <zsLib/XML.h>

namespace openpeer { namespace core { ZS_DECLARE_SUBSYSTEM(openpeer_core) } }

namespace openpeer
{
  namespace core
  {
    namespace internal
    {
      using services::IHelper;

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark ISettingsForStack
      #pragma mark

      //-----------------------------------------------------------------------
      void ISettingsForStack::applyDefaultsIfNoDelegatePresent()
      {
        SettingsPtr singleton = Settings::singleton();
        if (!singleton) return;
        singleton->applyDefaultsIfNoDelegatePresent();
      }

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark Settings
      #pragma mark

      //-----------------------------------------------------------------------
      Settings::Settings()
      {
        ZS_LOG_DETAIL(log("created"))
      }

      //-----------------------------------------------------------------------
      Settings::~Settings()
      {
        mThisWeak.reset();
        ZS_LOG_DETAIL(log("destroyed"))
      }

      //-----------------------------------------------------------------------
      SettingsPtr Settings::convert(ISettingsPtr settings)
      {
        return dynamic_pointer_cast<Settings>(settings);
      }

      //-----------------------------------------------------------------------
      SettingsPtr Settings::create()
      {
        SettingsPtr pThis(new Settings());
        pThis->mThisWeak = pThis;
        return pThis;
      }

      //-----------------------------------------------------------------------
      SettingsPtr Settings::singleton()
      {
        static SingletonLazySharedPtr<Settings> singleton(Settings::create());
        SettingsPtr result = singleton.singleton();
        if (!result) {
          ZS_LOG_WARNING(Detail, slog("singleton gone"))
        }
        return result;
      }
      
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark Settings => ISettings
      #pragma mark

      //-----------------------------------------------------------------------
      void Settings::setup(ISettingsDelegatePtr delegate)
      {
        {
          AutoRecursiveLock lock(mLock);
          mDelegate = delegate;

          ZS_LOG_DEBUG(log("setup called") + ZS_PARAM("has delegate", (bool)delegate))
        }

        stack::ISettings::setup(delegate ? mThisWeak.lock() : stack::ISettingsDelegatePtr());
      }
      
      //-----------------------------------------------------------------------
      void Settings::applyDefaults()
      {
        {
          AutoRecursiveLock lock(mLock);
          get(mAppliedDefaults) = true;
        }

        setString(OPENPEER_CORE_SETTING_STACK_CORE_THREAD_PRIORITY, "normal");
        setString(OPENPEER_CORE_SETTING_STACK_MEDIA_THREAD_PRIORITY, "real-time");

        stack::ISettings::applyDefaults();
      }

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark Settings => ISettingsForStack
      #pragma mark

      //-----------------------------------------------------------------------
      void Settings::applyDefaultsIfNoDelegatePresent()
      {
        {
          AutoRecursiveLock lock(mLock);
          if (mDelegate) return;
          if (mAppliedDefaults) return;
        }

        ZS_LOG_WARNING(Detail, log("To prevent issues with missing settings, the default settings are being applied. Recommend installing a settings delegate to fetch settings required from a externally."))

        applyDefaults();
      }

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark Settings => ISettingsDelegate
      #pragma mark

      //-----------------------------------------------------------------------
      String Settings::getString(const char *key) const
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return String();
        }

        return delegate->getString(key);
      }

      //-----------------------------------------------------------------------
      LONG Settings::getInt(const char *key) const
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return 0;
        }

        return delegate->getInt(key);
      }

      //-----------------------------------------------------------------------
      ULONG Settings::getUInt(const char *key) const
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return 0;
        }

        return delegate->getUInt(key);
      }

      //-----------------------------------------------------------------------
      bool Settings::getBool(const char *key) const
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return 0;
        }

        return delegate->getBool(key);
      }

      //-----------------------------------------------------------------------
      float Settings::getFloat(const char *key) const
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return 0;
        }

        return delegate->getFloat(key);
      }

      //-----------------------------------------------------------------------
      double Settings::getDouble(const char *key) const
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return 0;
        }

        return delegate->getDouble(key);
      }

      //-----------------------------------------------------------------------
      void Settings::setString(
                               const char *key,
                               const char *value
                               )
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return;
        }

        delegate->setString(key, value);
      }

      //-----------------------------------------------------------------------
      void Settings::setInt(
                            const char *key,
                            LONG value
                            )
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return;
        }

        delegate->setInt(key, value);
      }

      //-----------------------------------------------------------------------
      void Settings::setUInt(
                             const char *key,
                             ULONG value
                             )
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return;
        }

        delegate->setUInt(key, value);
      }

      //-----------------------------------------------------------------------
      void Settings::setBool(
                             const char *key,
                             bool value
                             )
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return;
        }

        delegate->setBool(key, value);
      }

      //-----------------------------------------------------------------------
      void Settings::setFloat(
                              const char *key,
                              float value
                              )
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return;
        }

        delegate->setFloat(key, value);
      }

      //-----------------------------------------------------------------------
      void Settings::setDouble(
                               const char *key,
                               double value
                               )
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return;
        }

        delegate->setDouble(key, value);
      }

      //-----------------------------------------------------------------------
      void Settings::clear(const char *key)
      {
        ISettingsDelegatePtr delegate;
        {
          AutoRecursiveLock lock(mLock);
          delegate = mDelegate;
          if (!delegate) return;
        }

        delegate->clear(key);
      }

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark Settings => (internal)
      #pragma mark

      //-----------------------------------------------------------------------
      Log::Params Settings::log(const char *message) const
      {
        ElementPtr objectEl = Element::create("core::Settings");
        IHelper::debugAppend(objectEl, "id", mID);
        return Log::Params(message, objectEl);
      }

      //-----------------------------------------------------------------------
      Log::Params Settings::slog(const char *message)
      {
        return Log::Params(message, "core::Settings");
      }

    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark ISettings
    #pragma mark

    //-------------------------------------------------------------------------
    void ISettings::setup(ISettingsDelegatePtr delegate)
    {
      internal::SettingsPtr singleton = internal::Settings::singleton();
      if (!singleton) return;
      singleton->setup(delegate);
    }

    //-------------------------------------------------------------------------
    void ISettings::setString(
                              const char *key,
                              const char *value
                              )
    {
      return stack::ISettings::setString(key, value);
    }

    //-------------------------------------------------------------------------
    void ISettings::setInt(
                           const char *key,
                           LONG value
                           )
    {
      return stack::ISettings::setInt(key, value);
    }

    //-------------------------------------------------------------------------
    void ISettings::setUInt(
                            const char *key,
                            ULONG value
                            )
    {
      return stack::ISettings::setUInt(key, value);
    }

    //-------------------------------------------------------------------------
    void ISettings::setBool(
                            const char *key,
                            bool value
                            )
    {
      return stack::ISettings::setBool(key, value);
    }

    //-------------------------------------------------------------------------
    void ISettings::setFloat(
                             const char *key,
                             float value
                             )
    {
      return stack::ISettings::setFloat(key, value);
    }

    //-------------------------------------------------------------------------
    void ISettings::setDouble(
                              const char *key,
                              double value
                              )
    {
      return stack::ISettings::setDouble(key, value);
    }

    //-------------------------------------------------------------------------
    void ISettings::clear(const char *key)
    {
      return stack::ISettings::clear(key);
    }

    //-------------------------------------------------------------------------
    bool ISettings::apply(const char *jsonSettings)
    {
      return stack::ISettings::apply(jsonSettings);
    }

    //-------------------------------------------------------------------------
    void ISettings::applyDefaults()
    {
      internal::SettingsPtr singleton = internal::Settings::singleton();
      if (!singleton) return;
      singleton->applyDefaults();
    }
    
  }
}
