#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <unordered_map>
#include "moviesdb.h"

#include <fstream>
#include <iostream>

mysqlpp::StoreQueryResult connect(std::string query_str) {
    // Connect to database with: database, server, userID, password
    mysqlpp::Connection myDB( , , , );
    // Create a query
    mysqlpp::Query query = myDB.query();
    query << query_str;
    mysqlpp::StoreQueryResult result = query.store();

    return result;
}

void getMovieListing(std::vector<std::string> &movies) {
	std::string query = "SELECT title FROM movies";

        mysqlpp::StoreQueryResult result = connect(query);
        // result is a 2D vector of std::string's

	std::cout << result.size() << std::endl;

        for(unsigned int row = 0; row < result.size(); row++){
                std::string title = result[row][0].c_str();
                movies.push_back(title);
        }

}

void getMovieInfo(const char * path, std::string &buf) {
/*
	query the database for a specific entry (WHERE title = bleh)
	parse the information needed from the database using database schema

	SCHEMA:
	+----------------------+------------+
	| Field                | Type       |
	+----------------------+------------+
	| pk                   | int(11)    |
	| comments             | text       |
	| budget               | bigint(11) |
	| genre                | text       |
	| homepage             | text       |
	| id                   | int(11)    |
	| keywords             | text       |
	| original_language    | text       |
	| original_title       | text       |
	| overview             | text       |
	| popularity           | int(11)    |
	| production_companies | text       |
	| production_countries | text       |
	| release_date         | text       |
	| revenue              | bigint(11) |
	| runtime              | int(11)    |
	| spoken_languages     | text       |
	| status               | text       |
	| tagline              | text       |
	| title                | text       |
	| vote_average         | int(11)    |
	| vote_count           | int(11)    |
	+----------------------+------------+

budget, genre, homepage, keywords, origional_language, origional_title, overview, popularity, production_companies, production_countries, release_date, revenu, runtime, spoken_language, status, tagline, vote_agerage, vote_count 

*/

	//(void) path;
	std::string headers_query = "DESCRIBE movies";
	std::string data_query = "SELECT * FROM movies WHERE title = ";
 	std::string path_str(path);

	path_str = path_str.substr(1); // There is a '/' at the beginning of the path and needs to be removed

	char C_data_query[data_query.length() + path_str.length()];
	sprintf(C_data_query, "%s'%s'", data_query.c_str(), path_str.c_str());

	data_query = std::string(C_data_query);

	std::stringstream output;


	output <<  headers_query << "\n";
	output <<  data_query << "\n";

	mysqlpp::StoreQueryResult headers_result = connect(headers_query);
	mysqlpp::StoreQueryResult data_result    = connect(data_query);

	mysqlpp::StoreQueryResult::const_iterator data_it;
	mysqlpp::StoreQueryResult::const_iterator header_it;

	//there should only be one row, but the loop is just a sanity check
	for(data_it = data_result.begin(); data_it != data_result.end(); ++data_it){
		mysqlpp::Row current_data = *data_it;

		// Column starts at 1 so pk is not recorded
		int col = 1;
		for(header_it = ++headers_result.begin(); header_it != headers_result.end(); ++header_it, col++){
			mysqlpp::Row header_name = *header_it;
			output << std::string(header_name[0])  << " :: "<< std::string(current_data[col]);
			/*
			if(std::string(header_name[0]) == std::string("comments")){
				output << "; " << session_comments[path];
			}
			*/
			output << "\n\n";
		}
	}

	buf += output.str();

}

void addComment(std::string path, std::string buf){
	std::string oldCommentQuery = "SELECT comments FROM movies WHERE title = ";
	std::string title = path.substr(1); // There is a '/' at the beginning of the path


	char C_query1[oldCommentQuery.length() + title.length()];
        sprintf(C_query1, "%s'%s'", oldCommentQuery.c_str(), title.c_str());
	oldCommentQuery = std::string(C_query1);

	mysqlpp::StoreQueryResult oldCommentResult = connect(oldCommentQuery);


	std::string oldComment;
	oldComment = std::string(oldCommentResult[0][0]);


	std::string updateClause = "UPDATE movies SET comments = ";
	std::string whereClause = " WHERE title = ";

	char C_query2[updateClause.length() + buf.length() + whereClause.length() + 2 + title.length()];
	sprintf(C_query2, "%s'%s'%s'%s'", updateClause.c_str(), buf.c_str(), whereClause.c_str(), title.c_str());
	std::string updateCommentQuery = std::string(C_query2);

	mysqlpp::StoreQueryResult updateCommentResult = connect(updateCommentQuery);

}
