#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <vector>
#include <string>
#include "sha256/sha256.h"

class users_blockchain
{
public:
  class userdata_block {
    struct block_data {
      int    id;
      char   username[32];
      time_t registred_time;

      block_data() {
        id = -1;
        username[0] = 0;
        registred_time = 0;
      }
      ~block_data() {}
    };

    std::string last_hash;
    std::string curr_hash;
    block_data data;
  public:
    userdata_block() {}
    userdata_block(const char *p_username, const std::string &_last_hash, int _id) {
      data.id = _id;
      data.registred_time = time(NULL);
      strncpy_s(data.username, p_username, sizeof(data.username));
      last_hash = _last_hash;
      curr_hash = calc_hash();
    }
    ~userdata_block() {}

    std::string calc_hash() const {
      char hash[64];
      sha256_buff buff;
      sha256_init(&buff);
      sha256_update(&buff, &data, sizeof(data));
      sha256_finalize(&buff);
      sha256_read_hex(&buff, hash);
      return std::string(hash, 64);
    }

    const std::string &get_hashsum() const { return curr_hash; }
    const std::string &get_last_block_hashsum() const { return last_hash; }

    /* for print */
    const int   get_user_id() const { return data.id; }
    const char *get_username() const { return data.username; }

    /* for corrupt */
    void corrupt_user_id(int newid) { data.id = newid; }
    void corrupt_username(const char *p_newname) {
      strncpy_s(data.username, p_newname, sizeof(data.username));
    }
  };

private:
  std::vector<userdata_block> blocks;

public:
  users_blockchain() {
    blocks.push_back(userdata_block("master_record", "0", 0));
  }
  ~users_blockchain() {}

  void add_user_block(const char *p_username) {
    size_t idx = blocks.size();
    const std::string &last_hash = blocks[idx - 1].get_hashsum();
    userdata_block block(p_username, last_hash, (int)idx);
    blocks.push_back(block);
  }

  bool is_valid_chain() const {
    for (size_t i = 1; i < blocks.size(); i++) {
      const userdata_block &curr = blocks[i];
      const userdata_block &last = blocks[i - 1];
      if (curr.get_hashsum() != curr.calc_hash()) {
        printf("is_valid_chain(): block %zd corrupted!\n", i);
        return false;
      }

      if (curr.get_last_block_hashsum() != last.get_hashsum()) {
        printf("is_valid_chain(): chain is corrupted in block %zd\n", i);
        return false;
      }
    }
    return true;
  }

  size_t get_size() { return blocks.size(); }
  const userdata_block &get_block(size_t idx) {
    assert(idx < get_size());
    return blocks[idx];
  }

  userdata_block &get_block_for_corrupt(size_t idx) {
    assert(idx < get_size());
    return blocks[idx];
  }

  void print_blockchain() {
    printf("--- print users blockchain ---\n");
    for (size_t i = 0; i < get_size(); i++) {
      const users_blockchain::userdata_block &user = get_block(i);
      printf("%zd: id(%d)  name(%s)\n", i, user.get_user_id(), user.get_username());
    }
  }
};

const char *get_random_username()
{
  static const char *p_usernames[] = {
    "Vasya",
    "Petya",
    "Alex",
    "Yuriy",
    "Evgeniy",
    "admin",
    "weaabasdb",
    "14fafa123",
    "useruser",
    "helloman"
  };
  return p_usernames[rand() % (sizeof(p_usernames) / sizeof(p_usernames[0]))];
}

int main()
{
  srand((unsigned int)time(NULL));
  users_blockchain blockchain; 
  for (size_t i = 0; i < 10; i++)
    blockchain.add_user_block(get_random_username());

  blockchain.print_blockchain();
  assert(blockchain.is_valid_chain());
  printf("SOURCE CHAIN IS %s\n", blockchain.is_valid_chain() ? "VALID" : "INVALID");

  users_blockchain::userdata_block &user = blockchain.get_block_for_corrupt(5);
  user.corrupt_username("vasillllllyi");
  user.corrupt_user_id(111);
  printf("MODIFIED CHAIN IS %s\n", blockchain.is_valid_chain() ? "VALID" : "INVALID");
  return 0;
}
