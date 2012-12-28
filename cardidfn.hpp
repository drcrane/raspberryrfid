#ifndef __CARDIDFN_HPP
#define __CARDIDFN_HPP

#include "sqlite3.h"
#include "stdio.h"

class User;
class UserService;

class CardService {
	private:
		sqlite3* db;
		sqlite3_stmt* getcard_stmt;
		UserService* userService;

	public:
		CardService(sqlite3 *db, UserService* userService);
		~CardService();
		int Add(unsigned long long int userid, void* number, size_t size);
		User* GetUserForCard(unsigned char* cardid, size_t cardidsz);
};

#endif // __CARDIDFN_HPP
