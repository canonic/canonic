#include "../include/Auth.hpp"

bool Auth::isUserSignedIn() const
{
    return this->m_signedIn;
}

void Auth::setUserSignedIn(bool signedIn)
{
    this->m_signedIn = signedIn;
    emit this->userSignedInChanged(this->m_signedIn);
}


QString Auth::getUserId() const
{
    return this->m_userId;
}

void Auth::setUserId(QString userId)
{
    this->m_userId = userId;
    emit this->userIdChanged(this->m_userId);
}

QString Auth::getDisplayName() const
{
    return this->m_displayName;
}

void Auth::setDisplayName(QString displayName)
{
    this->m_displayName = displayName;
    emit this->displayNameChanged(this->m_displayName);
}

QUrl Auth::getIdentityProvider() const
{
    return this->m_identityProvider;
}

void Auth::setIdentityProvider(QUrl identityProvider)
{
    this->m_identityProvider = identityProvider;
    emit this->identityProviderChanged(this->m_identityProvider);
}

QString Auth::getProofOfIdentityType() const
{
    return this->m_proofOfIdentityType;
}

void Auth::setProofOfIdentityType(QString proofOfIdentityType)
{
    this->m_proofOfIdentityType = proofOfIdentityType;
    emit this->proofOfIdentityTypeChanged(this->m_proofOfIdentityType);
}

QString Auth::getProofOfIdentity() const
{
    return this->m_proofOfIdentity;
}

void Auth::setProofOfIdentity(QString proofOfIdentity)
{
    this->m_proofOfIdentity = proofOfIdentity;
    emit this->proofOfIdentityChanged(this->m_proofOfIdentity);
}

QUrl Auth::getAvatarImageSource() const
{
    return this->m_avatarImageSource;
}

void Auth::setAvatarImageSource(QUrl avatarImageSource)
{
    this->m_avatarImageSource = avatarImageSource;
    emit this->avatarImageSourceChanged(this->m_avatarImageSource);
}

void Auth::signIn(QString userId,
    QString displayName,
    QUrl identityProvider,
    QString proofOfIdentityType,
    QString proofOfIdentity,
    QUrl avatarImgSource)
{
    this->setUserId(userId);
    this->setDisplayName(displayName);
    this->setIdentityProvider(identityProvider);
    this->setProofOfIdentityType(proofOfIdentityType);
    this->setProofOfIdentity(proofOfIdentity);
    this->setAvatarImageSource(avatarImgSource);
    this->setUserSignedIn(true);
}

void Auth::signOut()
{
    this->setUserId("");
    this->setDisplayName("");
    this->setIdentityProvider(QUrl(""));
    this->setProofOfIdentityType("");
    this->setProofOfIdentity("");
    this->setAvatarImageSource(QUrl(""));
    this->setUserSignedIn(false);
}
