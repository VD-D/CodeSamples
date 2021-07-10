FindKSimilarMovies <- function(userID, k, IsRow) {
  start.time <- Sys.time()
  # If user ID is not a character, make it a character.
  if (!is.character(userID)) {
    userID <- as.character(userID)
  }
  
  # Create a new empty list to store predicted ratings
  predicted.ratings <- list()
  
  # Extract the column/row from rating matrix based on user ID.
  if (IsRow == 1) {
    user.ratings <- rating.matrix[userID,]
    names(user.ratings) <- colnames(rating.matrix)
  } else {
    user.ratings <- rating.matrix[,userID]
    names(user.ratings) <- rownames(rating.matrix)
  }
  
  # Get both a vector containing all unrated movie IDs by that user and all rated movies.
  rated.movies   <- user.ratings[which(!is.na(user.ratings))]
  unrated.movies <- user.ratings[which(is.na(user.ratings))]
  
  # For each unrated movie, find the k most similar movies and try to estimate its rating
  # based on the user's past ratings.
  for (unrated.movie in names(unrated.movies)) {
    # Find the k + 1 most similar movies. The +1 accounts for the fact that one of the most similar
    # movies will be itself.
    sorted.movies <- as.list(movie.similarity.matrix[unrated.movie,])
    sorted.movies <- sort(unlist(sorted.movies), decreasing = TRUE)
    sorted.movies <- sorted.movies[sorted.movies > 0]
    sorted.k.movies <- names(sorted.movies)[1:(k + 1)]
    
    # Find out which k most similar movies have already been rated by the user.
    common.movies <- intersect(sorted.k.movies, names(rated.movies))
    if (length(common.movies) == 0) {
      predicted.ratings[unrated.movie] <- 0
    } else {
      predicted.ratings[unrated.movie] <- weighted.mean(rated.movies[common.movies], sorted.movies[common.movies])
    }
  }
  
  end.time <- Sys.time()
  print(end.time - start.time)
  
  return(predicted.ratings)
}

# States the required packages required for this project. Installs them if they
# are missing.
{

list.of.packages <- c("getopt", "Matrix", "reshape2", "parallel")
missing.packages <- list.of.packages[!(list.of.packages %in% installed.packages()[,"Package"])]

if (length(missing.packages) > 0) {
  install.packages(missing.packages)
}

# List of required libraries
library(getopt)
library(Matrix)
library(reshape2)
library(parallel)

# Test to see if all packages are installed - install.packages() may fail if the 
# user does not have an internet connection, for example.
uninstalled.packages <- list.of.packages[!(list.of.packages %in% installed.packages()[,"Package"])]
if (length(uninstalled.packages) > 0) { stop("Not all packages are installed!") }
else { print("Successfully loaded all packages!") }

# Parameter explanation:
# --- REQUIRED PARAMETERS ---
# MovieDataPath : a full path to a csv file containing a list of available movies.
#                 The expected column structure is: movieId, title, genres - the
#                 columns MUST be called this and are case sensitive.
#                 movieId: unique (integer) identifier of a movie.
#                 title: movie title.
#                 genres: genres of the move, delimited by GenreDelim. For example,
#                         if GenreDelim = "|" then a valid value for a row in genres
#                         is "Comedy|Animation|Family"
# NOTE ON FILTERING: If there are non-unique movieIds, the algorithm will take the first non-unique movieId
# it finds and remove all the others to ensure that all movieIds are unique. This may result in undesirable
# behavior.
#
# UserDataPath : a full path to a csv file containing a list of users and the movies
#               they watch. The expected column structure is: userId, movieId, rating, timestamp
#               - the columns MUST be called this and are case sensitive.
#               userId: integer identifying an individual user.
#               movieId: the movieId the user watched, as found in MovieData.
#               rating: the rating of the movie. Must be a value of 0.0 (poor) to 5.0 (good).
#               timestamp: integer value representing seconds since midnight 
#                         Coordinated Universal Time (UTC) of January 1, 1970.
# NOTE ON FILTERING: If a row contains a movieId which does not appear in MovieData, 
# it will be removed. Any rows which do not have a rating value in range of 0 to 5 will be removed.
#
# OutputPath : A path to a folder where UserRecommendation.csv will be written to. 
#              The user/system executing this script must have write permissions to this folder.
#
#
# UserToPredictFor : A string which matches one of the userIds provided in UserDataPath. IF THIS USERID CANNOT BE FOUND,
#                    OR IF THIS VALUE IS NULL, then the script terminates.
#
# --- OPTIONAL PARAMETERS ---
# NoMoviesToPred : The number of movies to predict for a user. Defaults to 10.
#
# ContentFilter        : A value which must either be "ibcf" or "ubcf". Defaults to "ibcf"
#                        ibcf - Item-Based Collaborative Filtering. Runs the item-based collaborative filtering algorithm.
#                               This usually performs better where there are many movies with low amount of ratings
#                               or it is more desirable to recommend niche or lesser-known movies.
#                        ubcf - User-Based Collaborative Filtering. Runs the user-based collaborative filtering algorithm.
#                               This usually performs better when many movies have high amounts of ratings, but
#                               often skews results to recommend more popular/highly watched movies.
#
# NumNearestItems - The number of most alike movies to search for when weighting a user preference. If less than
#                   2, will be set to 2. Defaults to 1000. Prints a warning if greater than total number movies in
#                   dataset.
#
#
# --- PARAMETERS RELEVANT TO PARAMETER TUNING AND OPTIMISATION ---
# UserFilterLvl : A value which may be 0, 1 or 2. Defaults to 1.
#                 0 - All ratings (including 0 ratings) will be used. Often skews results in such a way 
#                     that users may be recommended movies which simply are widely not rated, rather than movies they
#                     may prefer.
#                 1 - All non-zero ratings. Recommended to use this if data set has sparse user recommendations.
#                 2 - All non-zero ratings by a user who has rated more than MinRecommendNum movies.
#                     This is best used for data sets which have a few "trusted" users who often rate movies.
#                     Improves run time.
# NOTE ON FILTERING: If this is not 0, 1 or 2, this value will be set to 1.
#
# MinRecommendNum : Only relevant if UserFilterLvl = 2. The minimal number of movies a user must have rated in order 
#                   for them to be a "trusted" user. Defaults to 20.
# NOTE ON FILTERING: If this is less than 1, it will be set to 1. If this is above 100, a warning will be displayed.
#
# UseMinRatings : A value which may be 0 or 1. If 1, movies with less than MinRatingsNum across all users will be
#                 removed from the data set. This improves run-time performance and may give more accurate results,
#                 BUT will likely prevent lesser-known or frequently unrated movies to be recommended. Defaults to 0.
# NOTE ON FILTERING: If this is not 0 or 1, this value will be set to 0.
#
# MinRatingsNum : Only relevant if UseMinRatings == 1 The minimal number of ratings a movie has to 
#                 have across all users in order to be included in the data set. If this value is equal to or 
#                 less than 0, then there is no minimal number of required  ratings. If this is > 100, a warning 
#                 will be displayed. Defaults to 20.

options.matrix <- matrix(c("MovieDataPath",   "m", "1", "character",
                           "UserDataPath",    "u", "1", "character",
                           # "GenreInfoPath", "g", "1", "character", #Unused
                           "OutputPath",      "o", "1", "character",
                           "UserToPredictFor","p", "1", "character",
                           "GenreDelim",      "d", "2", "character",
                           "ContentFilter",   "c", "2", "character",
                           "NoMoviesToPred",  "n", "2", "numeric",
                           "NumNearestItems", "i", "2", "numeric",
                           "UserFilterLvl",   "f", "2", "numeric",
                           "MinRecommendNum", "r", "2", "numeric",
                           "UseMinRatings",   "h", "2", "numeric",
                           "MinRatingsNum",   "y", "2", "numeric")
                         , ncol = 4, byrow = TRUE)
args <- getopt(spec = options.matrix)

# A list of "fake" arguments used for debugging.
# args$MovieDataPath <- file.path("D:", "Downloads", "IMDB-Dataset", "movies.csv")
# args$UserDataPath  <- file.path("D:", "Downloads", "IMDB-Dataset", "ratings.csv")
# args$OutputPath    <- file.path("D:", "Downloads", "IMDB-Dataset")

# Defaults to use for various parameters.
if (is.null(args$UserToPredictFor)) { stop("Error! No user specified to make predictions for!") }
if (is.null(args$GenreDelim)) { args$GenreDelim <- "|" }
if (is.null(args$UserFilterLvl)) { args$UserFilterLvl <- 1 }
if (is.null(args$MinRecommendNum)) { args$MinRecommendNum <- 20 }
if (is.null(args$MinRatingsNum)) { args$MinRatingsNum <- 20 }
if (is.null(args$UseMinRatings)) { args$UseMinRatings <- 0 }
if (is.null(args$NumNearestItems)) { args$NumNearestItems <- 20 }
if (is.null(args$ContentFilter)) { args$ContentFilter <- "ibcf" }
if (is.null(args$NoMoviesToPred)) { args$NoMoviesToPred <- 10 }

# Tests to filter unwanted/garbage/meaningless arguments.
if (args$UserFilterLvl < 0 || args$UserFilterLvl > 2) { args$UserFilterLvl <- 1 }
if (args$MinRecommendNum < 1) { args$MinRecommendNum <- 1 }
if (args$NumNearestItems < 2) { args$NumNearestItems <- 2}
if (!args$ContentFilter %in% c("ibcf", "ubcf")) { args$ContentFilter <- "ibcf" }
if (args$UseMinRatings != 0 || args$UseMinRatings != 1) { args$UseMinRatings <- 0 }
if (args$MinRecommendNum > 100) { print(paste("Warning! Using a large value for MinRecommendNum:", as.character(args$MinRecommendNum))) }
if (args$MinRatingsNum > 100) { print(paste("Warning! Using a large value for MinRatingsNum:", as.character(args$MinRatingsNum))) }
if (args$NumNearestItems > 50) { print(paste("Warning! Using a large value for NumNearestItems:", as.character(args$NumNearestItems))) }

# Read in relevant csv files.
movie.data  <- read.csv(args$MovieDataPath, stringsAsFactors = FALSE)
rating.data <- read.csv(args$UserDataPath)

# Ensure that movie.data and rating.data column names are identical to what is expected.
# This doubles-up as a check to ensure the files have been read properly.
expected.movie.colnames <- c("movieId", "title", "genres")
expected.rating.colnames <- c("userId", "movieId", "rating", "timestamp")

if (!identical(expected.movie.colnames, colnames(movie.data))) {
  collapsed.expected.cols <- paste(expected.movie.colnames, collapse = ", ")
  collapsed.data.cols    <- paste(colnames(movie.data), collapse = ", ")
  stop(paste("Expected movie data column names to be:", collapsed.expected.cols, ". Instead found:", collapsed.data.cols))
}

if (!identical(expected.rating.colnames, colnames(rating.data))) {
  collapsed.expected.cols <- paste(expected.rating.colnames, collapse = ", ")
  collapsed.data.cols    <- paste(colnames(rating.data), collapse = ", ")
  stop(paste("Expected user data column names to be:", collapsed.expected.cols, ". Instead found:", collapsed.data.cols))
}

# Ensure that the passed user ID is valid.
if (!args$UserToPredictFor %in% as.character(unique(rating.data$userId))) {
  stop(paste("Cannot find user", args$UserToPredictFor, "in user data!"))
}

# Remove meaningless/unwanted data from the data.frames.
movie.data <- movie.data[!duplicated(movie.data$movieId),]
rating.data <- rating.data[(rating.data$movieId %in% movie.data$movieId),]

if (args$MinRatingsNum > 0 && args$UseMinRatings == 1) {
  no.ratings <- which(as.list(table(rating.data$movieId) >= args$MinRatingsNum) == TRUE)
  rating.data <- rating.data[(rating.data$movieId %in% as.numeric(no.ratings)),]
}

if (args$UserFilterLvl == 0) {
  rating.data <- rating.data[rating.data$rating >= 0,]
  rating.data <- rating.data[rating.data$rating <= 5,]
} else if (args$UserFilterLvl == 1) {
  rating.data <- rating.data[rating.data$rating >= 0.5,]
  rating.data <- rating.data[rating.data$rating <= 5,]
} else if (args$UserFilterLvl == 2) {
  rating.data <- rating.data[rating.data$rating >= 0.5,]
  rating.data <- rating.data[rating.data$rating <= 5,]
  user.ids <- table(rating.data$userId) >= args$MinRecommendNum
  user.ids <- as.list(user.ids)
  rating.data <- rating.data[(rating.data$userId %in% as.numeric(which(user.ids == TRUE))),]
}

if (nrow(rating.data) < 1) {
  stop("Error! Filter settings resulted in all data being filtered! Consider setting MinRatingsNum to a lower value,
       UseMinRatings to 0 or UserFilterLvl to 0")
}

# Get all listed genres and create matrices for movie genre and the genre search matrix.
movie.genres <- unique(unlist(strsplit(movie.data$genres, paste("[", args$GenreDelim, "]", sep = ""))))

genre.matrix <- matrix(data = 0, nrow = nrow(movie.data), ncol = length(movie.genres))
colnames(genre.matrix) <- movie.genres

genre.matrix <- cbind(movie.data[, c(1:2)], genre.matrix)

for (i in 1:nrow(movie.data)) {
  genres <- unlist(strsplit(movie.data$genres[i], paste("[", args$GenreDelim, "]", sep = "")))
  genre.matrix[i,][which(colnames(genre.matrix) %in% genres)] <- 1
}

genre.search.matrix <- t(genre.matrix)
genre.search.matrix <- genre.search.matrix[c(3:nrow(genre.search.matrix)),]
genre.search.matrix <- as.data.frame(genre.search.matrix)

# Calculate similarity/correlation (aka normalized cosine similarity) for movies.
# Rename row/col names to match movie Ids.
movie.similarity.matrix <- as.data.frame(cor(t(genre.matrix[,3:ncol(genre.matrix)]), method = "pearson"))
colnames(movie.similarity.matrix) <- genre.matrix$movieId
rownames(movie.similarity.matrix) <- genre.matrix$movieId

if (args$ContentFilter == "ibcf") {
  print("Using IBCF algorithm")
  
  # Create a rating matrix from a map of movie ID to user ID
  rating.matrix <- dcast(rating.data, movieId~userId, value.var = "rating", na.rm=FALSE)
  rownames(rating.matrix) <- rating.matrix$movieId
  rating.matrix <- rating.matrix[,-1]
  
  # Using the above, create a list of predictions for args$UserToPredictFor
  my.predicition <- sort(unlist(FindKSimilarMovies(args$UserToPredictFor, args$NumNearestItems, 0)), decreasing = TRUE)
  predicted.movies <- as.data.frame(movie.data[which(movie.data$movieId %in% names(my.predicition[1:args$NoMoviesToPred])),2:3])
  write.csv(predicted.movies, args$OutputPath, row.names = FALSE)
} else if (args$ContentFilter == "ubcf") {
  print("Using UBCF algorithm")

  # Create a rating matrix from a map of user ID to movie ID
  rating.matrix <- dcast(rating.data, movieId~userId, value.var = "rating", na.rm=FALSE)
  rating.matrix.rownames <- rating.matrix$movieId
  rating.matrix <- rating.matrix[,-1]
  rating.matrix <- data.frame(t(rating.matrix))
  colnames(rating.matrix) <- rating.matrix.rownames
  rownames(rating.matrix) <- as.character(unique(rating.data$userId))
  
  # Using the above, create a list of predictions for args$UserToPredictFor
  my.predicition <- sort(unlist(FindKSimilarMovies(args$UserToPredictFor, args$NumNearestItems, 1)), decreasing = TRUE)
  predicted.movies <- as.data.frame(movie.data[which(movie.data$movieId %in% names(my.predicition[1:args$NoMoviesToPred])),2:3])
  write.csv(predicted.movies, args$OutputPath, row.names = FALSE)
}

}

# print(args$MovieDataPath)

# References:
# https://www.r-bloggers.com/movie-recommendation-with-recommenderlab/
# https://rpubs.com/jeknov/movieRec
# https://data-flair.training/blogs/data-science-r-movie-recommendation/
# https://www.youtube.com/watch?v=h9gpufJFF-0