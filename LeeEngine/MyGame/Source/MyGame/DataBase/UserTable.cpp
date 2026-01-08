#include "CoreMinimal.h"
#include "UserTable.h"

UserTable::UserTable(SQLiteWrapper& DB) : DB(DB) {}

// 테이블 생성
bool UserTable::CreateTable()
{
	const char* Query = R"(
        CREATE TABLE IF NOT EXISTS Users (
            UserID TEXT PRIMARY KEY,
            PasswordHash TEXT NOT NULL,
            Salt TEXT NOT NULL,
            CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

	return DB.Execute(Query);
}

// 랜덤 Salt 생성
std::string UserTable::GenerateSalt(size_t Length)
{
	const char* Charset = 
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"!@#$%^&*";

	std::random_device RD;
	std::mt19937 Generator(RD());
	std::uniform_int_distribution<> Distribution(0, 69);

	std::string Salt;
	Salt.reserve(Length);

	for (size_t i = 0; i < Length; ++i)
	{
		Salt += Charset[Distribution(Generator)];
	}

	return Salt;
}

// SHA256 직접 구현
std::string UserTable::SHA256(const std::string& Data)
{
	// SHA256 상수
	static const uint32_t K[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	// 초기 해시 값
	uint32_t H[8] = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
	};

	// 메시지 패딩
	std::vector<uint8_t> Message(Data.begin(), Data.end());
	uint64_t OriginalLength = Message.size() * 8;

	Message.push_back(0x80);

	while ((Message.size() % 64) != 56)
	{
		Message.push_back(0x00);
	}

	for (int i = 7; i >= 0; --i)
	{
		Message.push_back((OriginalLength >> (i * 8)) & 0xFF);
	}

	// 메시지 처리
	for (size_t ChunkStart = 0; ChunkStart < Message.size(); ChunkStart += 64)
	{
		uint32_t W[64] = {0};

		for (int i = 0; i < 16; ++i)
		{
			W[i] = (Message[ChunkStart + i * 4] << 24) |
				(Message[ChunkStart + i * 4 + 1] << 16) |
				(Message[ChunkStart + i * 4 + 2] << 8) |
				(Message[ChunkStart + i * 4 + 3]);
		}

		for (int i = 16; i < 64; ++i)
		{
			uint32_t S0 = ((W[i-15] >> 7) | (W[i-15] << 25)) ^ 
				((W[i-15] >> 18) | (W[i-15] << 14)) ^ 
				(W[i-15] >> 3);
			uint32_t S1 = ((W[i-2] >> 17) | (W[i-2] << 15)) ^ 
				((W[i-2] >> 19) | (W[i-2] << 13)) ^ 
				(W[i-2] >> 10);
			W[i] = W[i-16] + S0 + W[i-7] + S1;
		}

		uint32_t A = H[0], B = H[1], C = H[2], D = H[3];
		uint32_t E = H[4], F = H[5], G = H[6], H_ = H[7];

		for (int i = 0; i < 64; ++i)
		{
			uint32_t S1 = ((E >> 6) | (E << 26)) ^ 
				((E >> 11) | (E << 21)) ^ 
				((E >> 25) | (E << 7));
			uint32_t CH = (E & F) ^ ((~E) & G);
			uint32_t Temp1 = H_ + S1 + CH + K[i] + W[i];
			uint32_t S0 = ((A >> 2) | (A << 30)) ^ 
				((A >> 13) | (A << 19)) ^ 
				((A >> 22) | (A << 10));
			uint32_t MAJ = (A & B) ^ (A & C) ^ (B & C);
			uint32_t Temp2 = S0 + MAJ;

			H_ = G;
			G = F;
			F = E;
			E = D + Temp1;
			D = C;
			C = B;
			B = A;
			A = Temp1 + Temp2;
		}

		H[0] += A; H[1] += B; H[2] += C; H[3] += D;
		H[4] += E; H[5] += F; H[6] += G; H[7] += H_;
	}

	// Hex 문자열로 변환
	std::ostringstream OSS;
	for (int i = 0; i < 8; ++i)
	{
		OSS << std::hex << std::setw(8) << std::setfill('0') << H[i];
	}

	return OSS.str();
}

// 비밀번호 해싱
std::string UserTable::HashPassword(
	const std::string& Password, 
	const std::string& Salt)
{
	std::string Combined = Password + Salt;
	return SHA256(Combined);
}

void UserTable::SetOnUserCreatedCallback(OnUserCreatedCallback Callback)
{
	OnUserCreated = Callback;
}

// 유저 등록
bool UserTable::RegisterUser(
	const std::string& UserID,
	const std::string& Password)
{
	if (UserExists(UserID))
	{
		return false;
	}

	std::string Salt = GenerateSalt();
	std::string PasswordHash = HashPassword(Password, Salt);

	auto Stmt = DB.Prepare(
		"INSERT INTO Users (UserID, PasswordHash, Salt) VALUES (?, ?, ?)"
	);

	Stmt->BindText(1, UserID);
	Stmt->BindText(2, PasswordHash);
	Stmt->BindText(3, Salt);

	if (!Stmt->Step())
	{
		return false;
	}

	// 유저 생성 성공 시 콜백 호출
	if (OnUserCreated)
	{
		return OnUserCreated(UserID);
	}

	return true;
}

// Salt 가져오기
std::optional<std::string> UserTable::GetUserSalt(const std::string& UserID)
{
	auto Stmt = DB.Prepare(
		"SELECT Salt FROM Users WHERE UserID = ?"
	);

	Stmt->BindText(1, UserID);

	if (Stmt->Step())
	{
		return Stmt->GetText(0);
	}

	return std::nullopt;
}

// 로그인 검증
bool UserTable::ValidateLogin(
	const std::string& UserID,
	const std::string& Password)
{
	auto SaltOpt = GetUserSalt(UserID);
	if (!SaltOpt.has_value())
	{
		return false;
	}

	std::string Salt = SaltOpt.value();
	std::string InputHash = HashPassword(Password, Salt);

	auto Stmt = DB.Prepare(
		"SELECT COUNT(*) FROM Users WHERE UserID = ? AND PasswordHash = ?"
	);

	Stmt->BindText(1, UserID);
	Stmt->BindText(2, InputHash);

	if (Stmt->Step())
	{
		return Stmt->GetInt(0) > 0;
	}

	return false;
}

// 유저 존재 확인
bool UserTable::UserExists(const std::string& UserID)
{
	auto Stmt = DB.Prepare(
		"SELECT COUNT(*) FROM Users WHERE UserID = ?"
	);

	Stmt->BindText(1, UserID);

	if (Stmt->Step())
	{
		return Stmt->GetInt(0) > 0;
	}
	return false;
}

// 유저 삭제
bool UserTable::DeleteUser(const std::string& UserID)
{
	auto Stmt = DB.Prepare("DELETE FROM Users WHERE UserID = ?");
	Stmt->BindText(1, UserID);

	return Stmt->Step();
}
