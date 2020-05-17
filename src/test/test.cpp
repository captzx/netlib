#include <iostream>

#include <cryptopp/cryptlib.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <cryptopp/pssr.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/scrypt.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

#include <xtools/Common.h>

#include <xtools/Crypto.h>
using namespace x::tool;
using namespace CryptoPP;
struct PBKDF_TestTuple
{
	byte purpose;
	unsigned int iterations;
	const char* hexPassword, * hexSalt, * hexDerivedKey;
};

bool TestPBKDF(KeyDerivationFunction& pbkdf, const PBKDF_TestTuple* testSet, unsigned int testSetSize)
{
	bool pass = true;

	for (unsigned int i = 0; i < testSetSize; i++)
	{
		const PBKDF_TestTuple& tuple = testSet[i];

		std::string password, salt, derivedKey;
		StringSource(tuple.hexPassword, true, new HexDecoder(new StringSink(password)));
		StringSource(tuple.hexSalt, true, new HexDecoder(new StringSink(salt)));
		StringSource(tuple.hexDerivedKey, true, new HexDecoder(new StringSink(derivedKey)));

		double timeInSeconds = 0.0f;
		AlgorithmParameters params = MakeParameters("Purpose", (int)tuple.purpose)
			(Name::Salt(), ConstByteArrayParameter(ConstBytePtr(salt), BytePtrSize(salt)))
			("Iterations", (int)tuple.iterations)("TimeInSeconds", timeInSeconds);

		SecByteBlock derived(derivedKey.size());
		pbkdf.DeriveKey(derived, derived.size(), ConstBytePtr(password), BytePtrSize(password), params);
		bool fail = !!memcmp(derived, derivedKey.data(), derived.size()) != 0;
		pass = pass && !fail;

		HexEncoder enc(new FileSink(std::cout));
		std::cout << (fail ? "FAILED   " : "passed   ");
		enc.Put(tuple.purpose);
		std::cout << " " << tuple.iterations;
		std::cout << " " << tuple.hexPassword << " " << tuple.hexSalt << " ";
		enc.Put(derived, derived.size());
		std::cout << std::endl;
	}

	return pass;
}

bool ValidatePBKDF()
{
	bool pass = true;

	{
		// from OpenSSL PKCS #12 Program FAQ v1.77, at http://www.drh-consultancy.demon.co.uk/test.txt
		PBKDF_TestTuple testSet[] =
		{
			{1, 1, "0073006D006500670000", "0A58CF64530D823F", "8AAAE6297B6CB04642AB5B077851284EB7128F1A2A7FBCA3"},
			{2, 1, "0073006D006500670000", "0A58CF64530D823F", "79993DFE048D3B76"},
			{1, 1, "0073006D006500670000", "642B99AB44FB4B1F", "F3A95FEC48D7711E985CFE67908C5AB79FA3D7C5CAA5D966"},
			{2, 1, "0073006D006500670000", "642B99AB44FB4B1F", "C0A38D64A79BEA1D"},
			{3, 1, "0073006D006500670000", "3D83C0E4546AC140", "8D967D88F6CAA9D714800AB3D48051D63F73A312"},
			{1, 1000, "007100750065006500670000", "05DEC959ACFF72F7", "ED2034E36328830FF09DF1E1A07DD357185DAC0D4F9EB3D4"},
			{2, 1000, "007100750065006500670000", "05DEC959ACFF72F7", "11DEDAD7758D4860"},
			{1, 1000, "007100750065006500670000", "1682C0FC5B3F7EC5", "483DD6E919D7DE2E8E648BA8F862F3FBFBDC2BCB2C02957F"},
			{2, 1000, "007100750065006500670000", "1682C0FC5B3F7EC5", "9D461D1B00355C50"},
			{3, 1000, "007100750065006500670000", "263216FCC2FAB31C", "5EC4C7A80DF652294C3925B6489A7AB857C83476"}
		};

		PKCS12_PBKDF<SHA1> pbkdf;

		std::cout << "\nPKCS #12 PBKDF validation suite running...\n\n";
		pass = TestPBKDF(pbkdf, testSet, COUNTOF(testSet)) && pass;
	}

	{
		// from draft-ietf-smime-password-03.txt, at http://www.imc.org/draft-ietf-smime-password
		PBKDF_TestTuple testSet[] =
		{
			{0, 5, "70617373776f7264", "1234567878563412", "D1DAA78615F287E6"},
			{0, 500, "416C6C206E2D656E746974696573206D75737420636F6D6D756E69636174652077697468206F74686572206E2d656E74697469657320766961206E2D3120656E746974656568656568656573", "1234567878563412","6A8970BF68C92CAEA84A8DF28510858607126380CC47AB2D"}
		};

		PKCS5_PBKDF2_HMAC<SHA1> pbkdf;

		std::cout << "\nPKCS #5 PBKDF2 validation suite running...\n\n";
		pass = TestPBKDF(pbkdf, testSet, COUNTOF(testSet)) && pass;
	}

	return pass;
}

int main() {
	auto result = SHA3_256Hash("captzx");
	std::cout << std::endl << std::endl << result << std::endl;
	return 0;
}