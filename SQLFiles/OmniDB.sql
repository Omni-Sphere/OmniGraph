drop database if exists [OmniDB]
go
CREATE DATABASE [OmniDB]
GO
USE [OmniDB]
GO
CREATE TABLE Sequences
(
	SeqEntry INT NOT NULL,
	UserSequence INT NOT NULL,
    SessionSequence INT NOT NULL,
	ItemSequence INT NOT NULL,
    ItemTreeTypeSequence INT NOT NULL,
    WarehouseSequence INT NOT NULL,
    ItemBrandsSequence INT NOT NULL,
    ItemGroupsSequence INT NOT NULL,
    CONSTRAINT PK_Sequences_Entry PRIMARY KEY (SeqEntry)
)
GO
CREATE TABLE GlobalConngiguration
(
    ConfEntry INT NOT NULL,
    ImagePath NVARCHAR(255),
    PDFPath NVARCHAR(255),
    XMLPath NVARCHAR(255),
    PasswordExpirationDays SMALLINT NOT NULL CONSTRAINT DF_GlobalConfiguration_PasswordNeverExpires DEFAULT 0,
    CONSTRAINT PK_Configs_ConfEntry PRIMARY KEY (ConfEntry),
    CONSTRAINT CHK_Configs_PasswordExpiration CHECK (PasswordExpirationDays >= 0)
)
GO
CREATE TABLE Users
(
    UserEntry INT NOT NULL,
    [Code] [NVARCHAR](20) NOT NULL,
    [Name] [NVARCHAR](50),
    Email NVARCHAR(50),
    Phone NVARCHAR(20),
    EmployeeEntry INT,
    [Password] VARBINARY(128) NOT NULL,
    SuperUser CHAR NOT NULL CONSTRAINT DF_Users_SuperUser DEFAULT 'N',
    IsLocked CHAR NOT NULL CONSTRAINT DF_Users_IsLocked DEFAULT 'N',
    IsActive CHAR NOT NULL CONSTRAINT DF_Users_IsActive DEFAULT 'N',
    ChangePasswordNextLogin CHAR NOT NULL CONSTRAINT DF_Users_ChangePasswordNextLogin DEFAULT 'Y',
    PasswordNeverExpires CHAR NOT NULL CONSTRAINT DF_Users_PasswordNeverExpires DEFAULT 'N',
    CreatedBy INT NOT NULL,
    CreateDate DATETIME NOT NULL,
    LastUpdatedBy INT,
    UpdateDate DATETIME,
    CONSTRAINT PK_Users_UserEntry PRIMARY KEY (UserEntry),
    CONSTRAINT UQ_Users_Code UNIQUE ([Code]),
    CONSTRAINT CHK_Users_IsLocked CHECK (IsLocked IN ('Y', 'N')),
    CONSTRAINT CHK_Users_SuperUser CHECK (SuperUser IN ('Y', 'N')),
    CONSTRAINT CHK_Users_PasswordNeverExpires CHECK (PasswordNeverExpires IN ('Y', 'N')),
    CONSTRAINT CHK_Users_ModifyPasswdNextLogin CHECK (ChangePasswordNextLogin IN ('Y', 'N')),
    INDEX IDX_Users_Code NONCLUSTERED ([Code]),
    INDEX IDX_Users_Name NONCLUSTERED ([Name])
)
GO
CREATE TABLE Sessions 
(
    SessionEntry INT NOT NULL,
    SessionUUID UNIQUEIDENTIFIER NOT NULL,
    UserCode NVARCHAR(20),
    UserEmail NVARCHAR(50),
    UserPhone NVARCHAR(20),
    DeviceIP NVARCHAR(50) NOT NULL,
    HostName NVARCHAR(100) NOT NULL,
    IsActive CHAR NOT NULL CONSTRAINT DF_Sessions_IsActive DEFAULT 'Y',
    StartDate DATETIME NOT NULL,
    EndDate DATETIME,
    Reason INT, -- -1 = Undefined, 0 = Logout, 1 = Timeout, 2 = Token expired, 3 = Forced, 4 = Device disconnected, 5 = Security event
    LogoutMessage NVARCHAR(254),
    DurationSeconds BIGINT,
    CONSTRAINT PK_Sessions_SessionEntry PRIMARY KEY (SessionEntry),
    INDEX IDX_Sessions_SessionUUID NONCLUSTERED (SessionUUID),
)
GO
CREATE TABLE Items 
(
    ItemEntry INT NOT NULL,
    [Code] NVARCHAR(50) NOT NULL,
    [Name] NVARCHAR(100) NOT NULL,
    [Description] NVARCHAR(254),
    [Image] NVARCHAR(254),
    IsActive CHAR NOT NULL CONSTRAINT DF_Items_IsActive DEFAULT 'Y',
    PurchaseItem CHAR NOT NULL CONSTRAINT DF_Items_PurchaseItem DEFAULT 'Y',
    SellItem CHAR NOT NULL  CONSTRAINT DF_Items_SellItem DEFAULT 'Y',
    InventoryItem CHAR NOT NULL CONSTRAINT DF_Items_InventoryItem DEFAULT 'Y',
    Price DECIMAL(19,6) NOT NULL CONSTRAINT DF_Items_Price DEFAULT 0.0,
    BrandEntry INT,
    GroupEntry INT,
    StockByWharehouse CHAR NOT NULL CONSTRAINT DF_Items_StockByWarehouse DEFAULT 'N',
    OnHand DECIMAL(19,6) NOT NULL CONSTRAINT DF_Items_OnHand DEFAULT 0.0,
    OnOrder DECIMAL(19,6) NOT NULL CONSTRAINT DF_Items_OnOrder DEFAULT 0.0,
    OnRequest DECIMAL(19,6) NOT NULL CONSTRAINT DF_Items_OnRequest DEFAULT 0.0,
    MinStock DECIMAL(19,6),
    MaxStock DECIMAL(19,6),
    MinOrder DECIMAL(19,6),
    MaxOrder DECIMAL(19,6),
    MinRequest DECIMAL(19,6),
    MaxRequest DECIMAL(19,6),
    CreatedBy INT NOT NULL,
    LastUpdatedBy DATETIME,
    CreateDate DATETIME NOT NULL,
    UpdateDate DATETIME,
    CONSTRAINT PK_Items_ItemEntry PRIMARY KEY (ItemEntry),
    CONSTRAINT UQ_Items_Code UNIQUE ([Code]),
    CONSTRAINT UQ_Items_Name UNIQUE ([Name]),
    CONSTRAINT CHK_Items_OnHand CHECK (OnHand >= 0),
    CONSTRAINT CHK_Items_OnOrder CHECK (OnOrder >= 0),
    CONSTRAINT CHK_Items_OnRequest CHECK (OnRequest >= 0),
    INDEX IDX_Items_Code NONCLUSTERED ([Code]),
    INDEX IDX_Items_Name NONCLUSTERED ([Name]),
)
GO
CREATE TABLE Warehouses
(
    WhsEntry INT NOT NULL,
    [Code] NVARCHAR(20) NOT NULL,
    [Name] NVARCHAR(100) NOT NULL,
    IsLocked CHAR NOT NULL CONSTRAINT DF_Warehouses_IsLocked DEFAULT 'Y',
    CreatedBy INT NOT NULL,
    LastUpdatedBy INT,
    CreateDate DATETIME NOT NULL,
    UpdateDate DATETIME,
    CONSTRAINT PK_Warehouses_WhsEntry PRIMARY KEY (WhsEntry),
    CONSTRAINT UQ_Warehouses_Code UNIQUE ([Code]),
    CONSTRAINT UQ_Warehouses_Name UNIQUE ([Name]),
    INDEX IDX_Warehouse_Code NONCLUSTERED ([Code]),
    INDEX IDX_Warehouse_Name NONCLUSTERED ([Name]),
)
GO
CREATE TABLE ItemsPerWarehouse
(
    ItmWhsEntry INT NOT NULL,
    ItemEntry INT NOT NULL,
    WarehouseEntry INT NOT NULL,
    IsLocked CHAR CONSTRAINT DF_ItemsPerWarehouse_IsLocked DEFAULT 'Y',
    OnHand DECIMAL(19,6) NOT NULL CONSTRAINT DF_ItemsPerWarehouse_OnHand DEFAULT 0.0,
    OnOrder DECIMAL(19,6) NOT NULL CONSTRAINT DF_ItemsPerWarehouse_OnOrder DEFAULT 0.0,
    OnRequest DECIMAL(19,6) NOT NULL CONSTRAINT DF_ItemsPerWarehouse_OnRequest DEFAULT 0.0,
    MinStock DECIMAL(19,6),
    MaxStock DECIMAL(19,6),
    MinOrder DECIMAL(19,6),
    MaxOrder DECIMAL(19,6),
    MinRequest DECIMAL(19,6),
    MaxRequest DECIMAL(19,6),
    CreatedBy INT NOT NULL,
    CreateDate DATETIME NOT NULL,
    UpdateDate DATETIME,
    CONSTRAINT PK_ItemsPerWarehouse_ItmsWhsEntry PRIMARY KEY (ItmWhsEntry),
    CONSTRAINT UQ_ItemsPerWarehouse_ItemWarehouse UNIQUE (ItemEntry, WarehouseEntry)
)
GO
CREATE TABLE ItemTreeTypes
(
    ITTEntry INT NOT NULL,
    ItemCode NVARCHAR(100) NOT NULL,
    [Description] NVARCHAR(254),
    Quantity DECIMAL(19,6) NOT NULL CONSTRAINT DF_ItemTreeTypes_Quantity DEFAULT 1.0,
    WrehouseEntry INT,
    TreeType CHAR NOT NULL CONSTRAINT DF_ItemTreeTypes_TreeType DEFAULT 'N', --A = Assembly, P = Production, S = Sales, T = Template
    CreatedBy INT NOT NULL,
    CreateDate DATETIME NOT NULL,
    UpdateDate DATETIME,
);
GO
CREATE TABLE ItemTreeTypeDetail
(
    ITTDetEntry INT NOT NULL,
    ITTEntry INT NOT NULL,
    LineNum INT NOT NULL,
    ItemEntry INT NOT NULL,
    WrehouseEntry INT,
    QtyRequired DECIMAL(19,6) NOT NULL,
    Price DECIMAL (19,6) NOT NULL,
    Comments NVARCHAR(200),
    CONSTRAINT PK_ItemTreeTypeDetail_ITTDetEntry PRIMARY KEY (ITTDetEntry),
    CONSTRAINT UQ_ItemTreeTypeDetail_ItemEntry UNIQUE(ITTEntry,ItemEntry),
    CONSTRAINT UQ_ItemTreeTypeDetail_LineNum UNIQUE(ITTEntry,LineNum)    
);
GO
CREATE TABLE ItemBrands
(
    ItBEntry INT NOT NULL,
    Code NVARCHAR(20) NOT NULL,
    [Name] NVARCHAR(100) NOT NULL,
    CreatedBy INT NOT NULL,
    LastUpdatedBy INT,
    CreateDate DATETIME NOT NULL,
    UpdateDate DATETIME,
    CONSTRAINT PK_ItemBrands_ItemBrandEntry PRIMARY KEY (ItBEntry),
    CONSTRAINT UQ_ItemBrands_Name UNIQUE ([Name]),
    CONSTRAINT UQ_ItemBrands_Code UNIQUE ([Code]),
    CONSTRAINT UQ_ItemBrands_CodeName UNIQUE ([Code],[Name]),
    INDEX IDX_ItemBrands_Name NONCLUSTERED ([Name]),
    INDEX IDX_ItemBrands_Code NONCLUSTERED ([Code])
)
GO
CREATE TABLE ItemGroups
(
    ItGEntry INT NOT NULL,
    Code NVARCHAR(20) NOT NULL,
    [Name] NVARCHAR(100) NOT NULL,
    CreatedBy INT NOT NULL,
    LastUpdatedBy INT,
    CreateDate DATETIME NOT NULL,
    UpdateDate DATETIME,
    CONSTRAINT PK_ItemGroups_ItemGroupEntry PRIMARY KEY (ItGEntry),
    CONSTRAINT UQ_ItemGroups_Name UNIQUE ([Name]),
    CONSTRAINT UQ_ItemGroups_Code UNIQUE ([Code]),
    CONSTRAINT UQ_ItemGroups_CodeName UNIQUE ([Code],[Name]),
    INDEX IDX_ItemGroups_Name NONCLUSTERED ([Name]),
    INDEX IDX_ItemGroups_Code NONCLUSTERED ([Code])
)
GO
delete from ItemGroups
delete from ItemBrands

update Sequences set ItemgroupsSequence = 0,ItemBrandsSequence = 0