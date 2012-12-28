#include "userfn.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

User::User(unsigned long long int id, char* friendlyName, unsigned char* jpegBuffer, unsigned long int jpegBufferSize) {
	this->id = id;
	this->friendlyName = friendlyName;
	this->jpegBufferLoaded = true;
	this->jpegBuffer = (unsigned char*)jpegBuffer;
	this->jpegBufferSize = jpegBufferSize;
}

User::~User() {
	free(this->friendlyName);
	if (this->jpegBuffer != NULL) {
		free(this->jpegBuffer);
	}
}

unsigned char* User::GetJpegBuffer() {
	if (!jpegBufferLoaded) {
		/* Load JPEG Buffer */
	}
	return this->jpegBuffer;
}

unsigned long int User::GetJpegBufferSize() {
	if (!jpegBufferLoaded) {
		/* Load JPEG Buffer */
	}
	return this->jpegBufferSize;
}

UserService::UserService(sqlite3 *db) {
	this->db = db;
	this->getuser_stmt = NULL;
}

int UserService::Add(User* user) {
	sqlite3_stmt * stmt;
	if (sqlite3_prepare(db, "insert into User values (?, ?, ?)", -1, // If less than zero, then stmt is read up to the first nul terminator
		&stmt, 0  // Pointer to unused portion of stmt
		) != SQLITE_OK) {
		return -1;
	}
	int res;
	res = sqlite3_bind_int64(stmt, 1, user->GetId());
	fprintf(stdout, "Res: %s\n", res == SQLITE_OK ? "OK" : "ERR");
	res = sqlite3_bind_text(stmt, 2, user->GetFriendlyName(), -1, SQLITE_STATIC);
	fprintf(stdout, "Res: %s\n", res == SQLITE_OK ? "OK" : "ERR");
	res = sqlite3_bind_blob(stmt, 3, user->GetJpegBuffer(), user->GetJpegBufferSize(), SQLITE_STATIC);
	fprintf(stdout, "Res: %s\n", res == SQLITE_OK ? "OK" : "ERR");
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		sqlite3_finalize(stmt);
		return -1;
	}
	sqlite3_finalize(stmt);
	return 0;
}

User* UserService::GetById(unsigned long long int id) {
	unsigned long long int iuserid;
	static const char* sql_stmt = "SELECT ID, FriendlyName, Picture FROM User WHERE ID = ?";
	if (getuser_stmt == NULL) {
		if (sqlite3_prepare(db, sql_stmt, -1, &getuser_stmt, 0) != SQLITE_OK) {
			return NULL;
		}
	}
	sqlite3_bind_int64(getuser_stmt, 1, id);
	int res;
	res = sqlite3_step(getuser_stmt);
	if (res == SQLITE_DONE) {
		return NULL;
	}
	if (res != SQLITE_ROW) {
		sqlite3_finalize(getuser_stmt);
		getuser_stmt = NULL;
		return NULL;
	}
	iuserid = sqlite3_column_int64(getuser_stmt, 0);

	char* friendlyname = NULL;
	char* ifriendlyname = (char *)sqlite3_column_text(getuser_stmt, 1);
	int friendlynamesize = sqlite3_column_bytes(getuser_stmt, 1);
	friendlyname = (char *)malloc(friendlynamesize + 1);
	memcpy (friendlyname, ifriendlyname, friendlynamesize);
	friendlyname[friendlynamesize] = 0;

	unsigned char* jpegbuffer = NULL;
	char* jpegimgbuf;
	int jpegimgbufsz;
	jpegimgbuf = (char *)sqlite3_column_blob(getuser_stmt, 2);
	jpegimgbufsz = sqlite3_column_bytes(getuser_stmt, 2);
	jpegbuffer = (unsigned char *)malloc(jpegimgbufsz);
	memcpy(jpegbuffer, jpegimgbuf, jpegimgbufsz);
	sqlite3_reset(getuser_stmt);
	return new User(id, friendlyname, jpegbuffer, jpegimgbufsz);
}

int UserService::DeleteById(unsigned long long int userid) {
	const char * sql_stmt = "DELETE FROM User WHERE ID = ? ;";
	sqlite3_stmt * stmt;
	if (sqlite3_prepare(db, sql_stmt, -1, &stmt, 0) != SQLITE_OK) {
		return -1;
	}
	sqlite3_bind_int64(stmt, 1, userid);
	int res;
	res = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	if (res != SQLITE_DONE) {
		return -1;
	}
	return 0;
}
