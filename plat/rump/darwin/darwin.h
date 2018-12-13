#include <stdint.h>
#include <sys/types.h>

#define CTL_KERN 1
#define CTL_HW 6

#define KERN_BOOTTIME    21
#define KERN_ARND    21

#define HW_PAGESIZE 7

/* note on 64 bit platforms, FreeBSD timespec is the same as NetBSD,
   but this is not true on 32 bit platforms */
/* XXX: need to redefine by darwin ones */
#define DARWIN_S_IFMT   0170000

#define DARWIN_S_IFIFO  0010000
#define DARWIN_S_IFCHR  0020000
#define DARWIN_S_IFDIR  0040000
#define DARWIN_S_IFBLK  0060000
#define DARWIN_S_IFREG  0100000
#define DARWIN_S_IFLNK  0120000
#define DARWIN_S_IFSOCK 0140000

#define DARWIN_S_ISDIR(mode)  (((mode) & DARWIN_S_IFMT) == DARWIN_S_IFDIR)
#define DARWIN_S_ISCHR(mode)  (((mode) & DARWIN_S_IFMT) == DARWIN_S_IFCHR)
#define DARWIN_S_ISBLK(mode)  (((mode) & DARWIN_S_IFMT) == DARWIN_S_IFBLK)
#define DARWIN_S_ISREG(mode)  (((mode) & DARWIN_S_IFMT) == DARWIN_S_IFREG)
#define DARWIN_S_ISFIFO(mode) (((mode) & DARWIN_S_IFMT) == DARWIN_S_IFIFO)
#define DARWIN_S_ISLNK(mode)  (((mode) & DARWIN_S_IFMT) == DARWIN_S_IFLNK)
#define DARWIN_S_ISSOCK(mode) (((mode) & DARWIN_S_IFMT) == DARWIN_S_IFSOCK)

typedef long darwin_time_t;

struct darwin_timespec {
	darwin_time_t tv_sec;
	long tv_nsec;
};
typedef uint32_t darwin_dev_t;
typedef uint64_t darwin_ino_t;
typedef uint32_t darwin_mode_t;
typedef uint32_t darwin_nlink_t;
typedef uint32_t darwin_uid_t;
typedef uint32_t darwin_gid_t;
typedef uint64_t darwin_off_t;
typedef uint64_t darwin_blkcnt_t;
typedef uint32_t darwin_blksize_t;

/* man fstat(2) */
struct darwin_stat { /* when _DARWIN_FEATURE_64_BIT_INODE is defined */
	darwin_dev_t           st_dev;           /* ID of device containing file */
	darwin_mode_t          st_mode;          /* Mode of file (see below) */
	darwin_nlink_t         st_nlink;         /* Number of hard links */
	darwin_ino_t           st_ino;           /* File serial number */
	darwin_uid_t           st_uid;           /* User ID of the file */
	darwin_gid_t           st_gid;           /* Group ID of the file */
	darwin_dev_t           st_rdev;          /* Device ID */
	struct darwin_timespec st_atimespec;     /* time of last access */
	struct darwin_timespec st_mtimespec;     /* time of last data modification */
	struct darwin_timespec st_ctimespec;     /* time of last status change */
	struct darwin_timespec st_birthtimespec; /* time of file creation(birth) */
	darwin_off_t           st_size;          /* file size, in bytes */
	darwin_blkcnt_t        st_blocks;        /* blocks allocated for file */
	darwin_blksize_t       st_blksize;       /* optimal blocksize for I/O */
	uint32_t        st_flags;         /* user defined flags for file */
	uint32_t        st_gen;           /* file generation number */
	int32_t         st_lspare;        /* RESERVED: DO NOT USE! */
	int64_t         st_qspare[2];     /* RESERVED: DO NOT USE! */
};


#define		IF_NAMESIZE	16
#define		IFNAMSIZ	IF_NAMESIZE

typedef uint8_t sa_family_t;
typedef	char *caddr_t;
typedef uint32_t u_int32_t;

struct sockaddr {
	unsigned char	sa_len;
	sa_family_t	sa_family;
	char		sa_data[14];
};

struct ifdevmtu {
	int	ifdm_current;
	int	ifdm_min;
	int	ifdm_max;
};

struct ifkpi {
	unsigned int	ifk_module_id;
	unsigned int	ifk_type;
	union {
		void		*ifk_ptr;
		int		ifk_value;
	} ifk_data;
};

struct	ifreq {
#ifndef IFNAMSIZ
#define	IFNAMSIZ	IF_NAMESIZE
#endif
	char	ifr_name[IFNAMSIZ];		/* if name, e.g. "en0" */
	union {
		struct	sockaddr ifru_addr;
		struct	sockaddr ifru_dstaddr;
		struct	sockaddr ifru_broadaddr;
		short	ifru_flags;
		int	ifru_metric;
		int	ifru_mtu;
		int	ifru_phys;
		int	ifru_media;
		int	ifru_intval;
		caddr_t	ifru_data;
		struct	ifdevmtu ifru_devmtu;
		struct	ifkpi	ifru_kpi;
		u_int32_t ifru_wake_flags;
		u_int32_t ifru_route_refcnt;
		int	ifru_cap[2];
		u_int32_t ifru_functional_type;
#define IFRTYPE_FUNCTIONAL_UNKNOWN	0
#define IFRTYPE_FUNCTIONAL_LOOPBACK	1
#define IFRTYPE_FUNCTIONAL_WIRED	2
#define IFRTYPE_FUNCTIONAL_WIFI_INFRA	3
#define IFRTYPE_FUNCTIONAL_WIFI_AWDL	4
#define IFRTYPE_FUNCTIONAL_CELLULAR	5
#define	IFRTYPE_FUNCTIONAL_INTCOPROC	6
#define IFRTYPE_FUNCTIONAL_LAST		6
	} ifr_ifru;
#define	ifr_addr	ifr_ifru.ifru_addr	/* address */
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-to-p link */
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address */
#ifdef __APPLE__
#define	ifr_flags	ifr_ifru.ifru_flags	/* flags */
#else
#define	ifr_flags	ifr_ifru.ifru_flags[0]	/* flags */
#define	ifr_prevflags	ifr_ifru.ifru_flags[1]	/* flags */
#endif /* __APPLE__ */
#define	ifr_metric	ifr_ifru.ifru_metric	/* metric */
#define	ifr_mtu		ifr_ifru.ifru_mtu	/* mtu */
#define	ifr_phys	ifr_ifru.ifru_phys	/* physical wire */
#define	ifr_media	ifr_ifru.ifru_media	/* physical media */
#define	ifr_data	ifr_ifru.ifru_data	/* for use by interface */
#define	ifr_devmtu	ifr_ifru.ifru_devmtu
#define	ifr_intval	ifr_ifru.ifru_intval	/* integer value */
#define	ifr_kpi		ifr_ifru.ifru_kpi
#define	ifr_wake_flags	ifr_ifru.ifru_wake_flags /* wake capabilities */
#define	ifr_route_refcnt ifr_ifru.ifru_route_refcnt /* route references count */
#define	ifr_reqcap	ifr_ifru.ifru_cap[0]	/* requested capabilities */
#define	ifr_curcap	ifr_ifru.ifru_cap[1]	/* current capabilities */
};


#define IOCPARM_SHIFT	13
#define IOCPARM_MASK	((1 << IOCPARM_SHIFT) - 1)
#define IOC_OUT		0x40000000
#define IOC_IN		0x80000000
#define IOC_INOUT	(IOC_IN|IOC_OUT)

#define _IOC(inout,group,num,len)       ((unsigned long) \
	((inout) | (((len) & IOCPARM_MASK) << 16) | ((group) << 8) | (num)))

#define _IOR(g,n,t)	_IOC(IOC_OUT,	(g), (n), sizeof(t))
#define _IOW(g,n,t)	_IOC(IOC_IN,	(g), (n), sizeof(t))
#define _IOWR(g,n,t)	_IOC(IOC_INOUT,	(g), (n), sizeof(t))

#define	DIOCGMEDIASIZE	_IOR('d', 129, off_t)
#define FIOASYNC	_IOW('f', 125, int)

