#define TP_ADJ(p) (p)

#define _IOC(a,b,c,d) ( ((a)<<30) | ((b)<<8) | (c) | ((d)<<16) )
#define _IOC_NONE  0U
#define _IOC_WRITE 1U
#define _IOC_READ  2U

#define _IO(a,b) _IOC(_IOC_NONE,(a),(b),0)
#define _IOW(a,b,c) _IOC(_IOC_WRITE,(a),(b),sizeof(c))
#define _IOR(a,b,c) _IOC(_IOC_READ,(a),(b),sizeof(c))
#define _IOWR(a,b,c) _IOC(_IOC_READ|_IOC_WRITE,(a),(b),sizeof(c))

#define ARCH_SET_FS 0x1002

#define LINUX_MAP_SHARED        0x01
#define LINUX_MAP_PRIVATE       0x02
#define LINUX_MAP_FIXED         0x10
#define LINUX_MAP_ANON          0x20
#define LINUX_MAP_STACK         0x20000
#define LINUX_MAP_HUGETLB       0x40000
#define LINUX_MAP_HUGE_SHIFT    26
#define LINUX_MAP_HUGE_2MB      (21 << LINUX_MAP_HUGE_SHIFT)
#define LINUX_MAP_HUGE_1GB      (30 << LINUX_MAP_HUGE_SHIFT)
#define LINUX_MAP_HUGE(sz)      LINUX_MAP_HUGETLB | \
	( sz == 2097152 ? LINUX_MAP_HUGE_2MB : 0 ) | \
	( sz == 1073741824 ? LINUX_MAP_HUGE_1GB : 0 )

struct linux_stat {
	unsigned long	st_dev;
	unsigned long	st_ino;
	unsigned long	st_nlink;
	unsigned int	st_mode;
	unsigned int	st_uid;
	unsigned int	st_gid;
	unsigned int	__pad0;
	unsigned long	st_rdev;
	long		st_size;
	long		st_blksize;
	long		st_blocks;
	unsigned long	st_atime;
	unsigned long	st_atime_nsec;
	unsigned long	st_mtime;
	unsigned long	st_mtime_nsec;
	unsigned long	st_ctime;
	unsigned long	st_ctime_nsec;
	long		__unused[3];
};
