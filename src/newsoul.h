/*  newsoul - A SoulSeek client written in C++
    Copyright (C) 2006-2007 Ingmar K. Steen (iksteen@gmail.com)
    Copyright 2008 little blue poney <lbponey@users.sourceforge.net>
    Karol 'Kenji Takahashi' Woźniak © 2013 - 2014

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

#ifndef __NEWSOUL_NEWSOUL_H__
#define __NEWSOUL_NEWSOUL_H__

#include <iostream>
#include <memory>
#include <signal.h>
#include <string>
#include <vector>

#include <glog/logging.h>

#include "component.h"
#include "config.h"
#include "sharesdb.h"
#include "NewNet/nnreactor.h"
#include "NewNet/nnrefptr.h"

namespace newsoul
{
    /* Forward definitions for classes we use for the class definition. */
    class CodesetManager;
    class ServerManager;
    class PeerManager;
    class DownloadManager;
    class UploadManager;
    class SearchManager;
    class IfaceManager;

    class Newsoul {
        // This is new interface
        const std::string version = "0.2.NewI";
        std::map<std::string, std::shared_ptr<Component>> _components;

    public:
        const std::vector<std::shared_ptr<Component>> getComponents(std::vector<std::string> names);
        // 'Til here

        /* Our strong references to the various components. */
        std::shared_ptr<NewNet::Reactor> m_Reactor;
        std::shared_ptr<CodesetManager> m_Codeset;
        std::shared_ptr<ServerManager> m_Server;
        std::shared_ptr<PeerManager> m_Peers;
        std::shared_ptr<DownloadManager> m_Downloads;
        std::shared_ptr<UploadManager> m_Uploads;
        std::shared_ptr<Config> _config;
        std::shared_ptr<SearchManager> m_Searches;
        std::shared_ptr<IfaceManager> m_Ifaces;
        SharesDB *_globalShares;
        SharesDB *_buddyShares;
        int m_Token;
        std::vector<std::string> mPrivilegedUsers;

        static Newsoul *_instance;
        static void handleSignals(int signal);

        /*!
         * Parses a single piece of CLI interface chain.
         * Setter variant, i.e. calls newsoul::Config::set.
         * It also increments the iterator.
         * It is implemented that way in order to allow some error checks.
         * \param keys Config keys of the piece.
         * \param i Pointer to the main parser iterator.
         * \param argc CLI arguments number.
         * \param argv CLI arguments array.
         * \param n Number of arguments to chop.
         */
        void parsePSet(std::initializer_list<const std::string> keys, int *i, int argc, char *argv[]);

        /*!
         * \see newsoul::Newsoul::parsePSset.
         * Boolean version. Reads yes or no and converts it to boolean value.
         */
        void parsePSetBool(std::initializer_list<const std::string> keys, int *i, int argc, char *argv[]);

        /*!
         * \see newsoul::Newsoul::parsePSet.
         * Adder version, i.e. calls newsoul::Config::add.
         */
        void parsePAdd(std::initializer_list<const std::string> keys, int *i, int argc, char *argv[]);

        /*!
         * \see newsoul::Newsoul::parsePSet.
         * Deleter version, i.e. calls newsoul::Config::del.
         */
        void parsePDel(std::initializer_list<const std::string> keys, int *i, int argc, char *argv[]);

        /*!
         */
        void parsePart(std::map<const std::string, std::function<void(const std::string &sarg)>> func, const std::string carg, int *i, int argc, char *argv[]);

        /*!
         * Parses parts and pieces belonging to set(s) command.
         * \param i Pointer to the main parser iterator.
         * \param argc CLI arguments number.
         * \param argv CLI arguments array.
         * \return True if we should finish parsing.
         */
        bool parseSet(int *i, int argc, char *argv[]);

        /*!
         * Parses parts and pieces belonging to database(d) command.
         * \see newsoul::Newsoul::parseSet.
         */
        bool parseDatabase(int *i, int argc, char *argv[]);

        /*!
         * Parses parts and pieces belonging to listeners(l) command.
         * \see newsoul::Newsoul::parseSet.
         */
        bool parseListeners(int *i, int argc, char *argv[]);

        /*!
         * Parses a list of argmuents given on command line
         * and performs necessary actions, like setting config options, etc.
         * \param argc Number of arguments.
         * \param argv List of argument strings.
         * \return True if we should proceed with event loop.
         */
        bool parseArgs(int argc, char *argv[]);

    public:
        Newsoul();
        ~Newsoul();

        int run(int argc, char *argv[]);

        /* Generate the next unique (within this session) token. Used for
           identifying peer sockets and transfers. */
        int token() {
            return ++m_Token;
        }

        /* Return a pointer to the reactor. */
        std::shared_ptr<NewNet::Reactor> reactor() const { return m_Reactor; }

        /* Return a pointer to the config manager. */
        std::shared_ptr<Config> config() const { return this->_config; }

        /* Return a pointer to the codeset manager (codeset translator). */
        std::shared_ptr<CodesetManager> codeset() const { return m_Codeset; }

        /* Return a pointer to the server manager (connection to the soulseek
           server). */
        std::shared_ptr<ServerManager> server() const { return m_Server; }

        /* Return a pointer to the peer manager (handles incoming connections
           and passive / reverse connection requests. */
        std::shared_ptr<PeerManager> peers() const { return m_Peers; }

        /* Return a pointer to the download manager. */
        std::shared_ptr<DownloadManager> downloads() const { return m_Downloads; }

        /* Return a pointer to the upload manager. */
        std::shared_ptr<UploadManager> uploads() const { return m_Uploads; }

        std::shared_ptr<SearchManager> searches() const { return m_Searches; }

        /* Return a pointer to the interface manager. */
        std::shared_ptr<IfaceManager> ifaces() { return m_Ifaces; }

        SharesDB *shares() const {
            return this->_globalShares;
        }

        SharesDB *buddyshares() const {
            return this->_buddyShares;
        }

        void LoadShares();
        void LoadDownloads();

        bool isBanned(const std::string u);
        bool isIgnored(const std::string u);
        bool isTrusted(const std::string u);
        bool isBuddied(const std::string u);
        bool isPrivileged(const std::string u);
        bool toBuddiesOnly();
        bool haveBuddyShares();
        bool trustingUploads();
        bool autoClearFinishedDownloads();
        bool autoClearFinishedUploads();
        bool autoRetryDownloads();
        bool privilegeBuddies();
        unsigned int upSlots();
        unsigned int downSlots();
        void addPrivilegedUser(const std::string &user);
        void setPrivilegedUsers(const std::vector<std::string> &users);
        void sendSharedNumber();
        bool isEnabledPrivRoom();
    };
}

#endif // __NEWSOUL_NEWSOUL_H__
