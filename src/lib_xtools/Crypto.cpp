#include "Crypto.h"

#include <cryptopp/cryptlib.h>
#include <cryptopp/rsa.h>
#include <cryptopp/randpool.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/asn.h>
#include <cryptopp/osrng.h>
#include <cryptopp/algparam.h>
#include <cryptopp/argnames.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>
#include <cryptopp/files.h>

#include "Logger.h"
using namespace CryptoPP;

void x::tool::GenerateRSAKey(std::string& privKey, std::string& pubKey) {
	RandomPool randPool;

	RSAES_PKCS1v15_Decryptor decryptor(randPool, 1024);
	HexEncoder hexPrivEncoder(new StringSink(privKey));
	decryptor.AccessMaterial().Save(hexPrivEncoder);
	hexPrivEncoder.MessageEnd();

	RSAES_PKCS1v15_Encryptor encryptor(decryptor);
	HexEncoder hexPubEncoder(new StringSink(pubKey));
	encryptor.AccessMaterial().Save(hexPubEncoder);
	hexPubEncoder.MessageEnd();
}

std::string x::tool::RSAEncrypt(const std::string& pubKey, const std::string& message) {
	StringSource pubString(pubKey, true, new HexDecoder);
	RSAES_PKCS1v15_Encryptor pub(pubString);

	RandomPool randPool;
	std::string result;
	StringSource(message, true, new PK_EncryptorFilter(randPool, pub, new HexEncoder(new StringSink(result))));
	return result;
}

std::string x::tool::RSADecrypt(const std::string& privKey, const std::string& ciphertext){
	StringSource privString(privKey, true, new HexDecoder);
	RSAES_PKCS1v15_Decryptor priv(privString);

	RandomPool randPool;
	std::string result;
	StringSource(ciphertext, true, new HexDecoder(new PK_DecryptorFilter(randPool, priv, new StringSink(result))));
	return result;
}

std::string x::tool::PBKDFEncrypt(const std::string& password, const std::string& salt) {
	std::string hexPassword, hexSalt;
	StringSource(password, true, new HexDecoder(new StringSink(hexPassword)));
	StringSource(salt, true, new HexDecoder(new StringSink(hexSalt)));

	AlgorithmParameters params = 
		MakeParameters("Purpose", 1) // mark: Purpose == 1, what is Purpose?
		(Name::Salt(), ConstByteArrayParameter(ConstBytePtr(hexSalt), BytePtrSize(hexSalt)))
		("Iterations", 1) // mark: Iterations == 1
		("TimeInSeconds", (double)0.0f); // pwdbased.h: DeriveKey -> CRYPTOPP_ASSERT(iterations > 0 || timeInSeconds > 0);

	SecByteBlock derived(24); // ??
	PKCS12_PBKDF<SHA1> pbkdf;
	pbkdf.DeriveKey(derived, derived.size(), ConstBytePtr(hexPassword), BytePtrSize(hexPassword), params);

	std::string result;
	HexEncoder enc(new StringSink(result));
	enc.Put(derived, derived.size());

	return result;
}

std::string x::tool::SHA3_256Hash(const std::string& password){
	std::string hexPassword;
	StringSource(password, true, new HexDecoder(new StringSink(hexPassword)));

	SHA3_256 hash;
	hash.Update(ConstBytePtr(hexPassword), BytePtrSize(hexPassword));
	SecByteBlock digest(hash.DigestSize());
	hash.TruncatedFinal(digest, digest.size());

	std::string result;
	HexEncoder enc(new StringSink(result));
	enc.Put(digest, digest.size());

	return result;
}