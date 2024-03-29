#ifndef __MYFS_H__
#define __MYFS_H__

#include <memory>
#include <vector>
#include <bitset>
#include <stdint.h>
#include "blkdev.h"

#define BLOCK_SIZE 4096
#define BLOCK_DATA_SIZE (BLOCK_SIZE - sizeof(uint32_t))

#define INODE_TABLE_BLOCKS 7

class MyFs
{
  public:
	MyFs(BlockDeviceSimulator *blkdevsim_);

	/**
	 * dir_list_entry struct
	 * This struct is used by list_dir method to return directory entry
	 * information.
	 */
	struct dir_list_entry
	{
		/**
		 * The directory entry name
		 */
		std::string name;

		/**
		 * whether the entry is a file or a directory
		 */
		bool is_dir;

		/**
		 * File size
		 */
		int file_size;
	};
	typedef std::vector<struct dir_list_entry> dir_list;

	struct myfs_entry
	{
		uint32_t inode;
		uint32_t first_block;
		uint32_t size;
		bool is_dir;
	};

	struct myfs_dir
	{
		uint32_t amount;
	};

	struct myfs_dir_entry
	{
		uint32_t inode;
		char name[10];
	};
	typedef std::vector<struct myfs_dir_entry> dir_entries;

	/**
	 * format method
	 * This function discards the current content in the blockdevice and
	 * create a fresh new MYFS instance in the blockdevice.
	 */
	void format();

	/**
	 * create_file method
	 * Creates a new file in the required path.
	 * @param path_str the file path (e.g. "/newfile")
	 * @param directory boolean indicating whether this is a file or directory
	 */
	void create_file(std::string path_str, bool directory);

	/**
	 * get_content method
	 * Returns the whole content of the file indicated by path_str param.
	 * Note: this method assumes path_str refers to a file and not a
	 * directory.
	 * @param path_str the file path (e.g. "/somefile")
	 * @return the content of the file
	 */
	std::string get_content(std::string path_str);

	/**
	 * set_content method
	 * Sets the whole content of the file indicated by path_str param.
	 * Note: this method assumes path_str refers to a file and not a
	 * directory.
	 * @param path_str the file path (e.g. "/somefile")
	 * @param content the file content string
	 */
	void set_content(std::string path_str, std::string content);

	/**
	 * list_dir method
	 * Returns a list of a files in a directory.
	 * Note: this method assumes path_str refers to a directory and not a
	 * file.
	 * @param path_str the file path (e.g. "/somedir")
	 * @return a vector of dir_list_entry structures, one for each file in
	 *	the directory.
	 */
	dir_list list_dir(std::string path_str);

	std::string change_directory(std::string path);

  private:
	/**
	 * This struct represents the first bytes of a myfs filesystem.
	 * It holds some magic characters and a number indicating the version.
	 * Upon class construction, the magic and the header are tested - if
	 * they both exist than the file is assumed to contain a valid myfs
	 * instance. Otherwise, the blockdevice is formated and a new instance is
	 * created.
	 */
	struct myfs_header
	{
		char magic[4];
		uint8_t version;
	};

	struct myfs_info
	{
		uint32_t inode_count;
		std::bitset<DEVICE_SIZE / BLOCK_SIZE> block_bitmap;
	};

	struct myfs_block
	{
		char data[BLOCK_DATA_SIZE];
		uint32_t next_block;
	};

	BlockDeviceSimulator *blkdevsim;

	uint32_t _current_dir_inode;

	static const uint8_t CURR_VERSION = 0x03;
	static const char *MYFS_MAGIC;

	std::string change_directory(std::string path, std::string dir_name);
	void create_dir(std::string path, std::string dir_name);
	void init_dir(struct myfs_entry *dir_entry, struct myfs_entry *prev_dir_entry, struct myfs_info *sys_info);
	std::string read_file(std::string path, std::string file_name);
	void deallocate_block_chain(uint32_t block_chain_head);
	void write_file(std::string path, std::string file_name, std::string content);
	void add_dir_entry(struct myfs_entry *dir, struct myfs_entry *file_entry, std::string file_name);
	void create_file(std::string path, std::string file_name);
	void update_entry(struct myfs_entry *file_entry);
	void add_entry(struct myfs_entry *file_entry);
	void update_file(struct myfs_entry *file_entry, char *data, uint32_t size);
	struct myfs_entry allocate_file(bool is_dir, struct myfs_info *sys_info);
	uint32_t allocate_block(struct myfs_block* block, struct myfs_info *sys_info);
	struct myfs_entry get_dir(const std::string &path_str);
	dir_entries get_dir_entries(myfs_entry dir_entry);
	struct myfs_entry get_file_entry(const uint32_t inode);
	void get_file(const myfs_entry file_entry, char *file_data);
};

#endif // __MYFS_H__
